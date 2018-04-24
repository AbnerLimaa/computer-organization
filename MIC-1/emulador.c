#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//Tipos de barramento do emulador
typedef unsigned char b_1bit;
typedef unsigned char b_8bits;
typedef unsigned int b_32bits;
typedef unsigned long int b_64bits;

//Numero de variaveis do programa que está sendo executado
static int numvar = 0;

//ALU
typedef struct alu
{
    b_32bits saida;
    b_1bit N;
    b_1bit Z;
} alu;

//Processador
typedef struct processador
{
    alu ALU;
    b_32bits MAR;
    b_32bits MDR;
    b_32bits PC;
    b_8bits MBR;
    b_32bits SP;
    b_32bits LV;
    b_32bits CPP;
    b_32bits TOS;
    b_32bits OPC;
    b_32bits H;
    b_32bits A;
    b_32bits B;
    b_32bits C;
    b_32bits MPC;
    b_64bits MIR;
    b_64bits microprograma[512];
    b_8bits *memoria;
} processador;

//Cria um novo processador com o tamanho de memoria especificado
processador *newproc(b_8bits *mem)
{
    processador *p = (processador*)malloc(sizeof(processador));
    if(p != NULL && mem != NULL)
    {
        p->MAR = 0;
        p->MDR = 0;
        p->PC = 0;
        p->MBR = 0;
        p->SP = 0;
        p->LV = 0;
        p->CPP = 0;
        p->TOS = 0;
        p->OPC = 0;
        p->H = 0;
        p->A = 0;
        p->B = 0;
        p->C = 0;
        p->MPC = 0;
        p->MIR = 0;
        p->ALU.saida = 0;
        p->ALU.N = 0;
        p->ALU.Z = 0;
        p->memoria = mem;
        return p;
    }
    return NULL;
}

//Libera da memoria um processador criado
void freeproc(processador *p)
{
    if(p != NULL)
    {
        free(p->memoria);
        free(p);
    }   
}

//Define qual registrador será usado no barramento B
void setbus(processador *p, b_8bits B)
{
    if(p != NULL)
    {
        p->A = p->H;
        switch(B)
        {
            case 0: p->B = p->MDR;
                break;
            case 1: p->B = p->PC;
                break;
            case 2: p->B = p->MBR;
                break;
            case 3: p->B = (signed int)p->MBR;
                break;
            case 4: p->B = p->SP;
                break;
            case 5: p->B = p->LV;
                break;
            case 6: p->B = p->CPP;
                break;
            case 7: p->B = p->TOS;
                break;
            case 8: p->B = p->OPC;
                break;
            default: break;
        }
    }
}

/* Opera a ALU, define o seu ultimo estado (N ou Z)
   e coloca o resultado no barramento C */
void opalu(processador *p, b_8bits controle)
{
    if(p != NULL)
    {
        switch(controle)
        {
            case 0b00011000: p->ALU.saida = p->A; 
                break;
            case 0b00010100: p->ALU.saida = p->B; 
                break;
            case 0b01010100: p->ALU.saida = p->B << 8;
                break;
            case 0b10010100: p->ALU.saida = p->B >> 8;
                break;
            case 0b11010100: p->ALU.saida = (p->B << 7) >> 1;
                break;
            case 0b00011010: p->ALU.saida = ~(p->A); 
                break;
            case 0b00101100: p->ALU.saida = ~(p->B); 
                break;
            case 0b00111100: p->ALU.saida = p->A + p->B; 
                break;
            case 0b00111101: p->ALU.saida = p->A + p->B + 1; 
                break;
            case 0b00111001: p->ALU.saida = p->A + 1; 
                break;
            case 0b00110101: p->ALU.saida = p->B + 1; 
                break;
            case 0b00111111: p->ALU.saida = p->B - p->A; 
                break;
            case 0b00110110: p->ALU.saida = p->B - 1; 
                break;
            case 0b00111011: p->ALU.saida = -(p->A); 
                break;
            case 0b00001100: p->ALU.saida = p->A & p->B; 
                break;
            case 0b00011100: p->ALU.saida = p->A | p->B; 
                break;
            case 0b00010000: p->ALU.saida = 0; 
                break;
            case 0b00110001: p->ALU.saida = 1; 
                break;
            case 0b00110010: p->ALU.saida = -1;
                break;
            default: break;
        }
        if(p->ALU.saida == 0)
        {
            p->ALU.N = 0;
            p->ALU.Z = 1;
        }
        else
        {
            p->ALU.N = 1;
            p->ALU.Z = 0;
        }
        p->C = p->ALU.saida;   
    } 
}

//Escolhe quais registradores receberao o valor do barramento C
void setreg(processador *p, b_32bits C)
{
    if(p != NULL)
    {
        if(C & 0x001)
            p->MAR = p->C;
        if(C & 0x002)
            p->MDR = p->C;
        if(C & 0x004)
            p->PC = p->C;
        if(C & 0x008)
            p->SP = p->C;
        if(C & 0x010)
            p->LV = p->C;
        if(C & 0x020)
            p->CPP = p->C;
        if(C & 0x040)
            p->TOS = p->C;
        if(C & 0x080)
            p->OPC = p->C;
        if(C & 0x100)
            p->H = p->C;
    }
}

/* Define o valor de MPC:
    1 - Salto incodicional: JAM-000
    2 - Salto se Z: JAM-001
    3 - Salto se N: JAM-010
    4 - Salto incondicional para posicao de MBR: JAM-100 */
void setMPC(processador *p, b_8bits JAM, b_32bits NEXT_ADDR)
{
    if(p != NULL)
    {
        p->MPC = NEXT_ADDR;
        if((JAM & 1) && p->ALU.Z)
            p->MPC = NEXT_ADDR | 0x100;
        if((JAM & 2) && p->ALU.N)
            p->MPC = NEXT_ADDR | 0x100;
        if(JAM & 4)
            p->MPC = p->MBR;
    }
}

/* Opera a memoria:
    1 - Fetch: MEM-001
    2 - Read: MEM-010
    3 - Write: MEM-100 */
void opmem(processador *p, b_8bits MEM)
{
    if(p != NULL)
    {
        if(MEM & 1)
            p->MBR = p->memoria[p->PC];
        if(MEM & 2)
            memcpy(&p->MDR, &p->memoria[p->MAR * 4], 4);
        if(MEM & 4)
            memcpy(&p->memoria[p->MAR * 4], &p->MDR, 4);
    }
}

/* Imprime na tela:
    1 - Registradores
    2 - Pilha de execucao
    3 - Variaveis do programa */
void console(processador *p)
{
    if(p != NULL)
    {
        int base = p->LV + numvar, var, elem;
        system("clear");
        printf("\t\tESTADO DO EMULADOR\t\t");
        printf("\nRegistradores:");
        printf("\n\tMAR: 0x%x", p->MAR);
        printf("\n\tMDR: 0x%x", p->MDR);
        printf("\n\t PC: 0x%x", p->PC);
        printf("\n\tMBR: 0x%x", p->MBR);
        printf("\n\t SP: 0x%x", p->SP);
        printf("\n\t LV: 0x%x", p->LV);
        printf("\n\tCPP: 0x%x", p->CPP);
        printf("\n\tTOS: 0x%x", p->TOS);
        printf("\n\tOPC: 0x%x", p->OPC);
        printf("\n\t  H: 0x%x\n", p->H);
        printf("\nMemoria:");
        printf("\nPilha:");
        for(int i = p->SP; i >= base; i--)
        {
            memcpy(&elem, &p->memoria[i * 4], 4);
            printf("\n\t0x%x: %d", i, elem);
        }
        printf("\nVariaveis Locais(0x%x):", p->LV);
        for(int i = 0; i < numvar; i++)
        {
            memcpy(&var, &p->memoria[(p->LV + i) * 4], 4);
            printf("\n\t%d: %d", i, var);
        }
        printf("\n");
    }
}

/* Realiza um ciclo de clock no processador:
    1 - Coloca em MIR a microinstrucao na posicao de MPC
    2 - Divide a microinstrucao em suas partes
    3 - escolhe o registrador para o barramento B
    4 - Opera a ALU
    5 - Coloca o resultado da ALU nos registradores que recebem o valor do barramento C
    6 - Opera a Memoria
    7 - Define o novo valor de MPC */
void clock(processador *p)
{
    if(p != NULL)
    {
        p->MIR = p->microprograma[p->MPC];
        b_32bits NEXT_ADDR = p->MIR >> 27;
        b_8bits JAM = (p->MIR << 37) >> 61;
        b_8bits ALU = (p->MIR << 40) >> 56;
        b_32bits C = (p->MIR << 48) >> 55;
        b_8bits MEM = (p->MIR << 57) >> 61;
        b_8bits B = (p->MIR << 60) >> 60;

        setbus(p, B);
        opalu(p, ALU);
        setreg(p, C);
        opmem(p, MEM);
        setMPC(p, JAM, NEXT_ADDR);
    }
}

//Carrega o microprograma de microprog.rom para p->microprograma
void loadmicroprog(processador *p, char *microprog)
{
    FILE *mp = fopen(microprog, "r");
    if(mp != NULL)
    {
        fread(p->microprograma, 8, 512, mp);
        fclose(mp);
    }
}

/* Calcula o numero de variaveis do programa executado.
   OBS: O calculo e feito de acordo com o valor maximo entre 
   as variaveis encontradas apos instrucoes ILOAD, pois 
   uma instrucao ILOAD admite uma variavel x para colocar
   no topo da pilha de execucao. */
int calcvar(char *prog, int tam)
{
    int counter = 0;
    int max = -1;
    for(int i = 0; i < tam; i++)
    {
        if(((int)prog[i] ==  0x1C) && (i < tam - 1))
            if((int)prog[i + 1] > max)
            {
                max = (int)prog[i + 1];
                counter++;
            } 
    }
    return counter;
}

/* Carrega o programa de prog.exe para p->memoria.
   OBS: Esse metodo carrega na memoria os 20 bytes de init
        caso prog.exe nao possua esses 20 bytes. */
void loadprog(processador *p, char *prog)
{
    FILE *pr = fopen(prog, "r");
    if(pr != NULL)
    {
        int numBytes = 0;
        char *init = (char*)calloc(20, 1);
        fseek(pr, 0, SEEK_END);
        numBytes = ftell(pr) - 24;
        char *exe = (char*)calloc(numBytes, 1);
        fseek(pr, 4, SEEK_SET);
        fread(init, 1, 20, pr);
        fread(exe, 1, numBytes, pr);
        numvar = calcvar(exe, numBytes);
        if(strcmp(init, "") == 0)
        {
            p->memoria[1] = 0x73;
            p->memoria[4] = 0x0006;
            b_32bits tmp = 0x1001;
            memcpy(&p->memoria[8], &tmp, 4);
            tmp = 0x0400;
            memcpy(&p->memoria[12], &tmp, 4);
            tmp = 0x1001 + numvar;
            memcpy(&p->memoria[16], &tmp, 4);
        }
        else
            memcpy(&p->memoria[0], init, 20);
        memcpy(&p->memoria[0x0401], exe, numBytes);
        free(init);
        free(exe);
        fclose(pr);
    }
}

//Mantem o estado de execucao do processador e so para se digitado 'e'
void executar(processador *p)
{
    if(p != NULL)
    {
        int i = 1;
        char c;
        while(i)
        {
            console(p);
            clock(p);
            c = getchar();
            if(c == 'e' || p->PC == p->LV)
                i = 0;
        }
    }
}

/* Logica principal do emulador: 
    1 - Cria a memoria do processador
    2 - Cria o processador
    3 - Analisa os argumentos da linha de comando para conferir se 
        os arquivos sao microprog.rom e prog.exe, independentemente da ordem
    4 - carrega microprog.rom e prog.exe
    5 - Inicia a execucao do programa.
    6 - Libera o processador da memoria */
int main(int argc, char *argv[])
{
    if(argc == 3 && argv[1] != argv[2])
    {
        b_8bits *memoria = (b_8bits*)calloc(8192, sizeof(b_8bits));
        processador *p = newproc(memoria);
        if(strcmp(argv[1], "microprog.rom") == 0 && strcmp(argv[2], "prog.exe") == 0)
        {
            loadmicroprog(p, argv[1]);
            loadprog(p, argv[2]);
            executar(p);
        }
        if(strcmp(argv[1], "prog.exe") == 0 && strcmp(argv[2], "microprog.rom") == 0)
        {
            loadmicroprog(p, argv[2]);
            loadprog(p, argv[1]);
            executar(p);
        }
        freeproc(p);
    }
    return 0;
}