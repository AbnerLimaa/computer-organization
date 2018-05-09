#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct assembler
{
    char *char_arquivo;
    char **ling_assembly;
    FILE *executavel;
    char *ling_maquina;
    char *variaveis;
    int n_var;
    int n_char;
    int n_assembly;
    int n_maq;
    int pos_inst;
    char *nome_prog;
} assembler;

assembler *newass()
{
    assembler *a = (assembler*)malloc(sizeof(assembler));
    if(a != NULL)
    {
        a->char_arquivo = NULL;
        a->ling_assembly = NULL;
        a->executavel = NULL;
        a->ling_maquina = NULL;
        a->variaveis = NULL;
        a->nome_prog = NULL;
        a->n_var = 0;
        a->n_char = 0;
        a->n_assembly = 0;
        a->n_maq = 0;
        a->pos_inst = 0;
        return a;
    }
    return NULL;
}

void freeass(assembler *a)
{
    if(a != NULL)
    {
        free(a->char_arquivo);
        for(int i = 0; i < a->n_assembly; i++)
            free(a->ling_assembly[i]);
        free(a->ling_assembly);
        free(a->ling_maquina);
        free(a->variaveis);
        free(a->nome_prog);
        free(a);
        a = NULL;
    }
}

/* Recebe uma string qualquer e a decodifica.
   Retorna o byte correspondente se a string for uma instrucao
   ou retorna 0x00 caso contrario. */
int decodificar(char *inst)
{
    if(inst != NULL)
    {
        if(strcmp(inst, "nop") == 0)
            return 0x01;
        if(strcmp(inst, "iadd") == 0)
            return 0x02;
        if(strcmp(inst, "isub") == 0)
            return 0x05;
        if(strcmp(inst, "iand") == 0)
            return 0x08;
        if(strcmp(inst, "ior") == 0)
            return 0x0B;
        if(strcmp(inst, "dup") == 0)
            return 0x0E;
        if(strcmp(inst, "pop") == 0)
            return 0x10;
        if(strcmp(inst, "swap") == 0)
            return 0x13;
        if(strcmp(inst, "bipush") == 0)
            return 0x19;
        if(strcmp(inst, "iload") == 0)
            return 0x1C;
        if(strcmp(inst, "istore") == 0)
            return 0x22;
        if(strcmp(inst, "wide") == 0)
            return 0x28;
        if(strcmp(inst, "ldc_w") == 0)
            return 0x32;
        if(strcmp(inst, "iinc") == 0)
            return 0x36;
        if(strcmp(inst, "goto") == 0)
            return 0x3C;
        if(strcmp(inst, "iflt") == 0)
            return 0x43;
        if(strcmp(inst, "ifeq") == 0)
            return 0x47;
        if(strcmp(inst, "if_icmpeq") == 0)
            return 0x4B;
        if(strcmp(inst, "invokevirtual") == 0)
            return 0x55;
        if(strcmp(inst, "ireturn") == 0)
            return 0x6B;
    }
    return 0x00;
}

/* Recebe uma instrucao decodificada e retorna a quantidade
   de bytes necessarios. */
int calcbytesop(int inst)
{
    if(inst == 0x02 || inst == 0x0E || inst == 0x08 || inst == 0x0B || inst == 0x6B || inst == 0x05 || inst == 0x01 || inst == 0x10 || inst == 0x13 || inst == 0x28)
        return 1;
    if(inst == 0x19 || inst == 0x1C || inst == 0x22)
        return 2;
    if(inst == 0x3C || inst == 0x47 || inst == 0x43 || inst == 0x55 || inst == 0x32 || inst == 0x4B || inst == 0x36)
        return 3;
    return 0;
}

/* Dado um label (offset) e a sua posicao no vetor ling_assembly,
   calcula quantos bytes de salto serao necessarios. */
int calcularsalto(assembler *a, char *offset, int i)
{
    if(offset != NULL && a != NULL && i >= 0 && i < a->n_assembly)
    {
        i++;
        int resultado = 2;
        while(i < a->n_assembly && strcmp(a->ling_assembly[i], offset) != 0)
        {
            int inst = decodificar(a->ling_assembly[i]);
            int bytes = calcbytesop(inst);
            resultado += bytes;
            i++;
        }
        if(i < a->n_assembly)
            return resultado + 1;
    }
    return -1;
}

// Retorna o inteiro correspondente a string passada como parametro
int transformarint(char *valor)
{
    if(valor != NULL)
    {
        int j = 1;
        int resultado = 0;
        for(int i = strlen(valor); i > 0; i--)
        {
            resultado = resultado + (((int)valor[i - 1] - 48) * j);
            j *= 10;
        }
        return resultado;
    }
    return -1;
}

/* Procura uma variavel no vetor variaveis.
   Retorna o indice da variavel no vetor se a encontra
   ou -1 caso contrario. */
int procurarvar(assembler *a, char var)
{
    if(a != NULL && a->variaveis != NULL && (int)var >= 97 && (int)var <= 122)
    {
        int i = 0;
        int continua = 1;
        while(i < a->n_var && continua)
        {
            if((int)a->variaveis[i] != (int)var)
                i++;
            else
                continua = 0;
        }
        if(i < a->n_var)
            return i;
    }
    return -1;
}

// Armazena uma instrucao decodificada em ling_maquina
void armazenarinst(assembler *a, int inst)
{
    if(a != NULL)
    {
        int n_bytes = calcbytesop(inst);
        a->ling_maquina = (char*)realloc(a->ling_maquina, (a->n_maq + n_bytes + 1) * sizeof(char));
        if(a->ling_maquina != NULL)
        {
            a->ling_maquina[a->n_maq] = inst;
            for(int i = 1; i < n_bytes; i++)
                a->ling_maquina[a->n_maq + i] = 0;
            a->pos_inst = a->n_maq + 1;
            a->n_maq += n_bytes;
        }
    }
}

// Armazena uma variavel nova no vetor variaveis
void armazenarvar(assembler *a, char var)
{
    if(a != NULL && a->ling_maquina != NULL)
    {
        a->variaveis = (char*)realloc(a->variaveis, (a->n_var + 1) * sizeof(char));
        a->variaveis[a->n_var] = var;
        a->ling_maquina[a->pos_inst] = a->n_var;
        a->n_var++;
    }
}

/* Armazena a operacao ou o operando (componente) no vetor ling_assembly.
   a componente e armazenada como string, pois ainda nao foi decodificada. */
void armazenarcomp(assembler *a, char *comp)
{
    if(a != NULL && comp != NULL)
    {
        a->ling_assembly = (char**)realloc(a->ling_assembly, (a->n_assembly + 1) * sizeof(char*));
        if(a->ling_assembly != NULL)
        {
            a->ling_assembly[a->n_assembly] = comp;
            a->n_assembly++;
        }
    }
}

/* Separa o vetor de caracteres char_arquivo em varias strings
   representando operacoes e operandos (componentes).
   Armazena o resultado em ling_assembly. */
void separarcomp(assembler *a)
{
    if(a != NULL)
    {
        char buffer[15];
        int j = 0;
        for(int i = 0; i < a->n_char; i++)
        {
            if(a->char_arquivo[i] != ' ' && a->char_arquivo[i] != '\n')
            {
                buffer[j] = a->char_arquivo[i];
                j++;
            }
            if((j != 0 && a->char_arquivo[i] == ' ') || a->char_arquivo[i] == '\n' || i + 1 == a->n_char)
            {   
                buffer[j] = '\0';
                char *comp = (char*)calloc(15, sizeof(char));
                strcpy(comp, buffer);
                armazenarcomp(a, comp);
                j = 0;
            }
        }
    }
}

/* Traduz os operandos e operacoes de ling_assembly e os armazena 
   em ling_maquina. Ao termino dessa funcao o vetor ling_maquina
   possui o programa montado. */
void traduzircomp(assembler *a)
{
    if(a != NULL)
    {
        int i = 0;
        while(i < a->n_assembly)
        {
            char *comp = a->ling_assembly[i];
            int dec = decodificar(comp);
            if(dec != 0)
            {
                armazenarinst(a, dec);
                i++;
            }
            else
            {
                int inst_ant = decodificar(a->ling_assembly[i - 1]);
                if(inst_ant == 0x19)
                    a->ling_maquina[a->pos_inst] = transformarint(comp);
                if(inst_ant == 0x1C || inst_ant == 0x22 || inst_ant == 0x36)
                {
                    int indice = procurarvar(a, comp[0]);
                    if(indice == -1)
                        armazenarvar(a, comp[0]);
                    else
                        a->ling_maquina[a->pos_inst] = indice;
                    if(inst_ant == 0x36)
                    {
                        int c = transformarint(a->ling_assembly[i + 1]);
                        a->ling_maquina[a->pos_inst + 1] = c;
                        i++;
                    }  
                }
                if(inst_ant == 0x32 || inst_ant == 0x55)
                {
                    int poscp = transformarint(a->ling_assembly[i]);
                    a->ling_maquina[a->pos_inst] = (poscp << 16) >> 24;
                    a->ling_maquina[a->pos_inst + 1] = (poscp << 24) >> 24;
                }
                if(inst_ant == 0x3C || inst_ant == 0x4B || inst_ant == 0x47 || inst_ant == 0x43)
                {
                    int salto = calcularsalto(a, comp, i);
                    a->ling_maquina[a->pos_inst] = (salto << 16) >> 24;
                    a->ling_maquina[a->pos_inst + 1] = (salto << 24) >> 24;
                }
                i++;
            }
        }
    }
}

/* Grava o resultado da montagem em um arquivo de saida seguindo
   o padrao:
   1 - 4 bytes com o tamanho do arquivo;
   2 - 20 bytes responsaveis por inicializar o emulador;
   3 - programa montado retirado de ling_maquina. */
void gravararquivo(assembler *a)
{
    if(a != NULL)
    {
        a->executavel = fopen(strcat(a->nome_prog, ".exe"), "wb");
        if(a->executavel != NULL)
        {
            int q = 20 + a->n_maq;
            char *init = (char*)calloc(20, sizeof(char));
            init[1] = 0x73;
            init[4] = 0x0006;
            unsigned int tmp = 0x1001;
            memcpy(&init[8], &tmp, 4);
            tmp = 0x0400;
            memcpy(&init[12], &tmp, 4);
            tmp = 0x1001 + a->n_var;
            memcpy(&init[16], &tmp, 4);
            fwrite(&q, 4, 1, a->executavel);
            fwrite(init, 1, 20, a->executavel);
            fwrite(a->ling_maquina, 1, a->n_maq, a->executavel);
            free(init);
            fclose(a->executavel);
        }
    }
}

/* Controla o processo de montagem do programa:
   1 - le todos os caracteres e os armazena em char_arquivo;
   2 - separa as strings de char_arquivo e as armazena em ling_assembly;
   3 - traduz as strings de ling_assembly e armazena o programa 
       resultante em ling_maquina. */
void montar(assembler *a, FILE *prog)
{
    if(a != NULL && prog != NULL)
    {
        fseek(prog, 0, SEEK_END);
        a->n_char = ftell(prog);
        fseek(prog, 0, SEEK_SET);
        a->char_arquivo = (char*)calloc(a->n_char, sizeof(char));
        fread(a->char_arquivo, 1, a->n_char, prog);
        separarcomp(a);
        traduzircomp(a);
        gravararquivo(a);
    }
}

//Nomeia o arquivo de saida com o mesmo arquivo de entrada
void nomearprog(assembler *a, char *programa)
{
    if(a != NULL && programa != NULL)
    {
        int i = 0;
        char *nome = (char*)calloc(1, sizeof(char));
        while(i < strlen(programa) && programa[i] != '.')
        {
            nome = (char*)realloc(nome, (i + 1) * sizeof(char));
            nome[i] = programa[i];
            i++;
        }
        nome[i] = '\0';
        a->nome_prog = (char*)calloc(i + 1, sizeof(char));
        strcpy(a->nome_prog, nome);
    }
}

int main(int argc, char *argv[])
{
    if(argc == 2 && strstr(argv[1], ".asm") != NULL)
    {

        FILE *prog = fopen(argv[1], "r");
        if(prog != NULL)
        {
            assembler *a = newass();
            nomearprog(a, argv[1]);
            montar(a, prog);
            freeass(a);
        }
        fclose(prog);
    }
    return 0;
}