#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int t = 2;

typedef struct ArvB {
    int nchaves, folha, *chave;
    char **filho; // vetor de nomes para arquivos
} TAB;

// seek em 0L: (nchaves) // seek em sizeof(int): (folha) // seek em sizeof(int)*2: vetor de chaves // seek em sizeof(int)*3: vetor dos nomes de arquivos
// para 0L e sizeof(int): ler 1 elemento // para sizeof(int)*2: ler 2*t-1 elementos // para sizeof(int)*3: ler 2*t elementos

char *Cria(char *nome, int t) { // criaï¿½ï¿½o de arquivo (criaï¿½ï¿½o de um nï¿½)
    FILE *arq = fopen(nome, "wb+");
    int i;
    TAB novo;
    novo.nchaves = 0;
    novo.folha = 1;
    novo.chave = (int*) malloc(sizeof (int)*((t * 2) - 1));

    for (i = 0; i < 2 * t - 1; i++) {
        novo.chave[i] = 0;
    } // inicializando todas as chaves com 0

    // Vetor dos nomes dos arquivos
    novo.filho = (char**) malloc(sizeof (char*)*t * 2);
    for (i = 0; i < (2 * t); i++) {
        novo.filho[i] = (char*) malloc(sizeof (char)*31);
    }
    //

    for (i = 0; i < (2 * t); i++) novo.filho[i] = "-1"; // inicializando todos os nomes de arquivo com -1
    fwrite(&novo, sizeof (TAB), 1, arq);
    fclose(arq);
    return nome;
}

void Imprime(char *nome, int andar) {
    TAB no;
    FILE *arq = fopen(nome, "rb");

    if (arq) {
        fseek(arq, 0, SEEK_SET);
        fread(&no, sizeof (TAB), 1, arq);
        fclose(arq);
        int i, j;
        for (i = 0; i < no.nchaves; i++) {
            Imprime(no.filho[i], andar + 1);
            for (j = 0; j < andar; j++) printf("   ");
            printf("%d\n", no.chave[i]);
        }
        Imprime(no.filho[i], andar + 1);
    } else {
        fclose(arq);
    }
}

FILE *Busca(char* nome, int ch) {
    TAB x;
    FILE *arq = fopen(nome, "rb");
    FILE *resp = NULL;
    if (!arq) return resp;
    fseek(arq, 0L, SEEK_SET);
    fread(&x, sizeof (TAB), 1, arq);

    int i = 0;
    while (i < x.nchaves && ch > x.chave[i]) i++;
    if (i < x.nchaves && ch == x.chave[i]) return arq;
    fclose(arq);
    if (x.folha) return resp;
    return Busca(x.filho[i], ch);
}

int verificar_nome(char *df, TAB x) {
    int i;
    for (i = 0; i < 2 * t; i++) {
        if (*df == *x.filho[i]) {
            return 0;
        }
    }

    return 1;
}

char *Divisao(char *x, int i, char *y, int *o, int t) {
    int temp = *o;
    TAB conteudo_x, conteudo_y, conteudo_z;

    FILE *arq = fopen(x, "rb");
    fseek(arq, 0L, SEEK_SET);
    fread(&conteudo_x, sizeof (TAB), 1, arq);
    fclose(arq);

    arq = fopen(y, "rb");
    fseek(arq, 0L, SEEK_SET);
    fread(&conteudo_y, sizeof (TAB), 1, arq);
    fclose(arq);

    char *df = (char*) malloc(sizeof (char)*51);

    temp++;
    sprintf(df, "%d-", temp);
    strcat(df, y);
    char *z;

    ///GAMBIRRA PARA Q Z NAO TENHO O MESMO NOME QUE UM FILHO DE X
    if (verificar_nome(df, conteudo_x)) {
        z = Cria(df, t);
    } else {
        temp++;
        sprintf(df, "%d-", temp);
        strcat(df, y);
        z = Cria(df, t);
    }
    ///////////////////////////

    arq = fopen(z, "rb");
    fseek(arq, 0L, SEEK_SET);
    fread(&conteudo_z, sizeof (TAB), 1, arq);
    fclose(arq);

    conteudo_z.nchaves = t - 1;
    conteudo_z.folha = conteudo_y.folha;

    int j;
    for (j = 0; j < t - 1; j++) conteudo_z.chave[j] = conteudo_y.chave[j + t];
    if (!conteudo_y.folha) {
        for (j = 0; j < t; j++) {
            conteudo_z.filho[j] = conteudo_y.filho[j + t];
            conteudo_y.filho[j + t] = "-1";
        }
    }

    conteudo_y.nchaves = t - 1;

    for (j = conteudo_x.nchaves; j >= i; j--) conteudo_x.filho[j + 1] = conteudo_x.filho[j];
    conteudo_x.filho[i] = z;
    for (j = conteudo_x.nchaves; j >= i; j--) conteudo_x.chave[j] = conteudo_x.chave[j - 1];
    conteudo_x.chave[i - 1] = conteudo_y.chave[t - 1];
    conteudo_x.nchaves++;

    arq = fopen(z, "wb");
    fseek(arq, 0L, SEEK_SET);
    fwrite(&conteudo_z, sizeof (TAB), 1, arq);
    fclose(arq);

    arq = fopen(y, "wb");
    fseek(arq, 0L, SEEK_SET);
    fwrite(&conteudo_y, sizeof (TAB), 1, arq);
    fclose(arq);

    arq = fopen(x, "wb");
    fseek(arq, 0L, SEEK_SET);
    fwrite(&conteudo_x, sizeof (TAB), 1, arq);
    fclose(arq);

    // printf("%s  %s  %s  %s\n",conteudo_x.filho[0],conteudo_x.filho[1],conteudo_x.filho[2],conteudo_x.filho[3]);

    *o = temp;
    return x;
}

char *Insere_Nao_Completo(char *nome, int k, int *o, int t) {
    FILE *arq = NULL;
    TAB x, x_filho;
    int temp = *o;

    arq = fopen(nome, "rb");
    fseek(arq, 0L, SEEK_SET);
    fread(&x, sizeof (TAB), 1, arq);
    fclose(arq);

    int i = x.nchaves - 1;
    if (x.folha) {
        while ((i >= 0) && (k < x.chave[i])) {
            x.chave[i + 1] = x.chave[i];
            i--;
        }
        x.chave[i + 1] = k;
        x.nchaves++;
        arq = fopen(nome, "wb");
        fseek(arq, 0L, SEEK_SET);
        fwrite(&x, sizeof (TAB), 1, arq);
        fclose(arq);

        return nome;
    }

    while ((i >= 0) && (k < x.chave[i])) i--;
    i++;

    // leitura do nï¿½mero de chaves do arquivo filho
    arq = fopen(x.filho[i], "rb");
    fseek(arq, 0L, SEEK_SET);
    fread(&x_filho, sizeof (TAB), 1, arq);
    fclose(arq);

    if (x_filho.nchaves == ((2 * t) - 1)) { // x_filho.nchaves = nchaves do filho (x->filho[i]->nchaves) 

        nome = Divisao(nome, (i + 1), x.filho[i], &temp, t);

        arq = fopen(nome, "rb");
        fseek(arq, 0L, SEEK_SET);
        fread(&x, sizeof (TAB), 1, arq);
        fclose(arq);

        if (k > x.chave[i]) i++;
    }

    x.filho[i] = Insere_Nao_Completo(x.filho[i], k, &temp, t);
    *o = temp;
    return nome;
}

char *Insere(char *nome, int k, int *o, int t) {
    FILE *T = NULL;
    TAB a;
    int temp = *o;
    char *ind, *end_s = NULL, *end_t = nome;
    ind = (char*) malloc(sizeof (char)*51);
    if (Busca(end_t, k)) return end_t;
    T = fopen(end_t, "rb");

    if (!T) {
        end_t = Cria(end_t, t);

        T = fopen(end_t, "rb");
        fseek(T, 0L, SEEK_SET);
        fread(&a, sizeof (TAB), 1, T);
        fclose(T);

        a.chave[0] = k;
        a.nchaves = 1;

        T = fopen(end_t, "wb");
        fseek(T, 0L, SEEK_SET);
        fwrite(&a, sizeof (TAB), 1, T);
        fclose(T);

        return end_t;
    }

    fseek(T, 0L, SEEK_SET);
    fread(&a, sizeof (TAB), 1, T);
    fclose(T);

    if (a.nchaves == (2 * t) - 1) {
        temp++;
        //criando o nome do novo arquivo
        sprintf(ind, "%d-", temp);
        strcat(ind, nome);
        end_s = Cria(ind, t);

        FILE *S = fopen(end_s, "rb");
        fseek(S, 0L, SEEK_SET);
        fread(&a, sizeof (TAB), 1, S);
        fclose(S);

        a.nchaves = 0;
        a.folha = 0;
        a.filho[0] = end_t;

        S = fopen(end_s, "wb");
        fseek(S, 0L, SEEK_SET);
        fwrite(&a, sizeof (TAB), 1, S);
        fclose(S);

        end_s = Divisao(end_s, 1, end_t, &temp, t);
        end_s = Insere_Nao_Completo(end_s, k, &temp, t);
        *o = temp;
        return end_s;
    }

    *o = temp;
    end_t = Insere_Nao_Completo(end_t, k, &temp, t);
    return end_t;
}

void imprime_no(TAB no) {
    int i;
    for (i = 0; i <= no.nchaves - 1; i++) {
        printf("Chave %d: %d\n", (i + 1), no.chave[i]);
    }
    for (i = 0; i <= no.nchaves; i++) {
        printf("Filho %d: %s\n", (i + 1), no.filho[i]);
    }
    printf("\n");
}

char* remover(char* nome, int ch, int t) {
    FILE *arq_raiz;
    arq_raiz = fopen(nome, "rb+");
    if (!arq_raiz) return nome;

    TAB raiz;
    fseek(arq_raiz, 0L, SEEK_SET);
    fread(&raiz, sizeof (TAB), 1, arq_raiz);
    int i;
    printf("Removendo %d...\n", ch);
    for (i = 0; i < raiz.nchaves && raiz.chave[i] < ch; i++);
    if (i < raiz.nchaves && ch == raiz.chave[i]) { //CASOS 1, 2A, 2B e 2C
        //CASO 1
        if (raiz.folha) {
            printf("\nCASO 1\n");
            int j;
            for (j = i; j < raiz.nchaves - 1; j++) raiz.chave[j] = raiz.chave[j + 1];
            raiz.nchaves--;
            fseek(arq_raiz, 0L, SEEK_SET);
            fwrite(&raiz, sizeof (TAB), 1, arq_raiz);
            fclose(arq_raiz);
            return nome;
        }

        //CASO 2A
        FILE* arq_filho_esq = fopen(raiz.filho[i], "rb+");
        TAB filho_esq;
        fseek(arq_filho_esq, 0L, SEEK_SET);
        fread(&filho_esq, sizeof (TAB), 1, arq_filho_esq);
        if (!raiz.folha && filho_esq.nchaves >= t) {
            printf("\nCASO 2A\n");
            //Encontrar o predecessor k' de k na Ã¡rvore com raiz em filho_esq
            char* predecessor;
            while (!filho_esq.folha) {
                predecessor = filho_esq.filho[filho_esq.nchaves];
                fclose(arq_filho_esq);
                arq_filho_esq = fopen(predecessor, "rb+");
                fseek(arq_filho_esq, 0L, SEEK_SET);
                fread(&filho_esq, sizeof (TAB), 1, arq_filho_esq);
            }
            fclose(arq_filho_esq);
            int temp = filho_esq.chave[filho_esq.nchaves - 1];
            //Eliminar recursivamente K e substitua K por K' em x
            raiz.filho[i] = remover(raiz.filho[i], temp, t);
            raiz.chave[i] = temp;
            fseek(arq_raiz, 0L, SEEK_SET);
            fwrite(&raiz, sizeof (TAB), 1, arq_raiz);
            fclose(arq_raiz);
            return nome;
        }
        //deixo o arq_filho_esq aberto caso nÃ£o entre no 2A, pois pode cair no 2C

        //CASO 2B
        TAB filho_dir;
        FILE* arq_filho_dir;
        if (i < raiz.nchaves) {
            arq_filho_dir = fopen(raiz.filho[i + 1], "rb+");
            fseek(arq_filho_dir, 0L, SEEK_SET);
            fread(&filho_dir, sizeof (TAB), 1, arq_filho_dir);
        }
        if (!raiz.folha && filho_dir.nchaves >= t) {
            printf("\nCASO 2B\n");
            //Encontrar o sucessor k' de k na Ã¡rvore com raiz em y
            char *sucessor;
            while (!filho_dir.folha) {
                sucessor = filho_dir.filho[filho_dir.nchaves];
                fclose(arq_filho_dir);
                arq_filho_dir = fopen(sucessor, "rb+");
                fseek(arq_filho_dir, 0L, SEEK_SET);
                fread(&filho_dir, sizeof (TAB), 1, arq_filho_dir);
            }

            int temp = filho_dir.chave[0];
            //Eliminar recursivamente K e substitua K por K' em x
            raiz.filho[i + 1] = remover(raiz.filho[i + 1], temp, t);
            raiz.chave[i] = temp;
            fseek(arq_raiz, 0L, SEEK_SET);
            fwrite(&raiz, sizeof (TAB), 1, arq_raiz);

            //se entrar aki o arq_filho_esq ainda estah aberto, fecha-lo antes de retorna-lo...
            fclose(arq_filho_esq);
            fclose(arq_filho_dir);
            fclose(arq_raiz);

            return nome;
        }
        fclose(arq_filho_dir);

        //CASO 2C
        if (!raiz.folha && filho_dir.nchaves == t - 1 && filho_esq.nchaves == t - 1) {
            printf("\nCASO 2C\n");
            filho_esq.chave[filho_esq.nchaves] = ch; //colocar ch ao final de filho[i]
            int j;
            for (j = 0; j < t - 1; j++) //juntar chave[i+1] com chave[i]
                filho_esq.chave[t + j] = filho_dir.chave[j];
            for (j = 0; j <= t; j++) //juntar filho[i+1] com filho[i]
                filho_esq.filho[t + j] = filho_dir.filho[j];
            filho_esq.nchaves = 2 * t - 1;
            for (j = i; j < raiz.nchaves - 1; j++) //remover ch de arv
                raiz.chave[j] = raiz.chave[j + 1];
            for (j = i + 1; j <= raiz.nchaves; j++) { //remover ponteiro para filho[i+1]
                raiz.filho[j] = raiz.filho[j + 1];
            }
            raiz.nchaves--;

            //escrever o nÃ³ filho antes da recursÃ£o, caso contrario o arquivo estÃ¡ desatualizado
            fseek(arq_filho_esq, 0L, SEEK_SET);
            fwrite(&filho_esq, sizeof (TAB), 1, arq_filho_esq);
            fclose(arq_filho_esq);

            raiz.filho[i] = remover(raiz.filho[i], ch, t);

            fseek(arq_raiz, 0L, SEEK_SET);
            fwrite(&raiz, sizeof (TAB), 1, arq_raiz);
            fclose(arq_raiz);

            return nome;
        }
    }
    // A PARTIR DAQUI!
    //CASOS 3
    TAB *no = malloc(sizeof (TAB)), *irmao = NULL, *aux = malloc(sizeof (TAB));
    FILE* arq_no = fopen(raiz.filho[i], "rb+");
    FILE* arq_irmao;
    if (i < raiz.nchaves) {
        arq_irmao = fopen(raiz.filho[i + 1], "rb+");
        //aux eh o irmÃ£o Ã  direita
        fseek(arq_irmao, 0L, SEEK_SET);
        fread(aux, sizeof (TAB), 1, arq_irmao);
    }
    fseek(arq_no, 0L, SEEK_SET);
    fread(no, sizeof (TAB), 1, arq_no);

    if (no->nchaves == t - 1) { //CASOS 3A e 3B

        if ((i < raiz.nchaves) && (aux->nchaves >= t)) { //CASO 3A
            printf("\nCASO 3A: i menor que nchaves\n");
            irmao = aux;
            no->chave[t - 1] = raiz.chave[i]; //dar ao nÃ³ a chave i do pai
            no->nchaves++;
            raiz.chave[i] = irmao->chave[0]; //dar ao pai uma chave do irmÃ£o do nÃ³
            int j;
            for (j = 0; j < irmao->nchaves - 1; j++) //ajustar chaves do irmÃ£o
                irmao->chave[j] = irmao->chave[j + 1];
            no->filho[no->nchaves] = irmao->filho[0]; //enviar ponteiro menor do irmao para o novo elemento no nÃ³
            for (j = 0; j < irmao->nchaves; j++) { //ajustar filhos do irmÃ£o
                irmao->filho[j] = irmao->filho[j + 1];
            }
            //PODE DAR MERDA AKIE
            irmao->filho[j] = "-1";
            irmao->nchaves--;
            //escrever os nÃ³s irmÃ£os antes da recursÃ£o, caso contrario os arquivos estarÃ£o desatualizados
            fseek(arq_irmao, 0L, SEEK_SET);
            fwrite(irmao, sizeof (TAB), 1, arq_irmao);
            fclose(arq_irmao);

            fseek(arq_no, 0L, SEEK_SET);
            fwrite(no, sizeof (TAB), 1, arq_no);
            fclose(arq_no);

            raiz.filho[i] = remover(raiz.filho[i], ch, t);

            fseek(arq_raiz, 0L, SEEK_SET);
            fwrite(&raiz, sizeof (TAB), 1, arq_raiz);

            fclose(arq_irmao);
            fclose(arq_no);
            fclose(arq_raiz);

            free(irmao);
            free(no);

            return nome;
        }

        if (!arq_irmao)
            fclose(arq_irmao);
        if (i > 0) {
            arq_irmao = fopen(raiz.filho[i - 1], "rb+");
            //aux eh o irmÃ£o Ã  esquerda
            fseek(arq_irmao, 0L, SEEK_SET);
            fread(aux, sizeof (TAB), 1, arq_irmao);
        }
        if ((i > 0) && (!irmao) && (aux->nchaves >= t)) { //CASO 3A
            printf("\nCASO 3A:"
                    ""
                    " i igual a nchaves\n");
            irmao = aux;
            int j;
            for (j = no->nchaves; j > 0; j--) //encaixar lugar da nova chave
                no->chave[j] = no->chave[j - 1];
            for (j = no->nchaves + 1; j > 0; j--) //encaixar lugar dos filhos da nova chave
                no->filho[j] = no->filho[j - 1];

            no->chave[0] = raiz.chave[i - 1]; //dar ao nÃ³ a chave i do pai
            no->nchaves++;
            raiz.chave[i - 1] = irmao->chave[irmao->nchaves - 1]; //dar ao pai uma chave do irmao do nÃ³
            no->filho[0] = irmao->filho[irmao->nchaves]; //enviar ponteiro do irmÃ£o para o novo elemento no nÃ³
            irmao->filho[irmao->nchaves] = "-1";
            irmao->nchaves--;

            fseek(arq_irmao, 0L, SEEK_SET);
            fwrite(irmao, sizeof (TAB), 1, arq_irmao);
            fclose(arq_irmao);

            fseek(arq_no, 0L, SEEK_SET);
            fwrite(no, sizeof (TAB), 1, arq_no);
            fclose(arq_no);

            raiz.filho[i] = remover(raiz.filho[i], ch, t);

            fseek(arq_raiz, 0L, SEEK_SET);
            fwrite(&raiz, sizeof (TAB), 1, arq_raiz);

            fclose(arq_irmao);
            fclose(arq_no);
            fclose(arq_raiz);

            free(irmao);
            free(no);

            return nome;
        }
        if (!arq_irmao)
            fclose(arq_irmao);
        if (i < raiz.nchaves) {
            arq_irmao = fopen(raiz.filho[i + 1], "rb+");
            fseek(arq_irmao, 0L, SEEK_SET);
            fread(aux, sizeof (TAB), 1, arq_irmao);
        }
        if (!irmao) { //CASO 3B
            if (i < raiz.nchaves && aux->nchaves == t - 1) {
                printf("\nCASO 3B: i menor que nchaves\n");
                irmao = aux;
                no->chave[t - 1] = raiz.chave[i]; //pegar chave [i] e coloca ao final de filho[i]
                no->nchaves++;
                int j;
                for (j = 0; j < t - 1; j++) {
                    no->chave[t + j] = irmao->chave[j]; //passar filho[i+1] para filho[i]
                    no->nchaves++;
                }
                if (!no->folha) {
                    for (j = 0; j < t; j++) {
                        no->filho[t + j] = irmao->filho[j];
                    }
                }

                //A PARTIR DAQUI O NÃ“ DA DIREITA EH EXCLUIDO...
                fseek(arq_irmao, 0L, SEEK_SET);
                fwrite(irmao, sizeof (TAB), 1, arq_irmao);
                fclose(arq_irmao);
                remove(raiz.filho[i + 1]);

                for (j = i; j < raiz.nchaves - 1; j++) { //limpar referÃªncias de i
                    raiz.chave[j] = raiz.chave[j + 1];
                    raiz.filho[j + 1] = raiz.filho[j + 2];
                }
                raiz.nchaves--;

                fseek(arq_no, 0L, SEEK_SET);
                fwrite(no, sizeof (TAB), 1, arq_no);
                fclose(arq_no);

                fseek(arq_raiz, 0L, SEEK_SET);
                fwrite(&raiz, sizeof (TAB), 1, arq_raiz);

                fclose(arq_no);
                fclose(arq_raiz);

                free(irmao);
                free(no);

                nome = remover(nome, ch, t);
                return nome;
            }
            if (!arq_irmao)
                fclose(arq_irmao);
            if (i > 0) {
                arq_irmao = fopen(raiz.filho[i - 1], "rb+");
                fseek(arq_irmao, 0L, SEEK_SET);
                fread(aux, sizeof (TAB), 1, arq_irmao);
            }
            //q serve para guardar o nome do arquivo do filho i-1
            char* q;
            if ((i > 0) && (aux->nchaves == t - 1)) {
                printf("\nCASO 3B: i igual a nchaves\n");
                irmao = aux;
                q = raiz.filho[i - 1];
                if (i == raiz.nchaves)
                    irmao->chave[t - 1] = raiz.chave[i - 1]; //pegar chave[i] e poe ao final de filho[i-1]
                else
                    irmao->chave[t - 1] = raiz.chave[i]; //pegar chave [i] e poe ao final de filho[i-1]
                irmao->nchaves++;
                int j;
                for (j = 0; j < t - 1; j++) {
                    irmao->chave[t + j] = no->chave[j]; //passar filho[i+1] para filho[i]
                    irmao->nchaves++;
                }
                if (!irmao->folha) {
                    for (j = 0; j < t; j++) {
                        irmao->filho[t + j] = no->filho[j];
                    }
                }

                //A PARTIR DAQUI O NÃ“ DA ESQUERDA EH EXCLUIDO...
                fseek(arq_no, 0L, SEEK_SET);
                fwrite(no, sizeof (TAB), 1, arq_no);
                fclose(arq_no);
                remove(raiz.filho[i]);

                raiz.nchaves--;
                raiz.filho[i - 1] = q;

                fseek(arq_irmao, 0L, SEEK_SET);
                fwrite(irmao, sizeof (TAB), 1, arq_irmao);

                fseek(arq_raiz, 0L, SEEK_SET);
                fwrite(&raiz, sizeof (TAB), 1, arq_raiz);

                fclose(arq_irmao);
                fclose(arq_raiz);

                nome = remover(nome, ch, t);

                free(irmao);
                free(no);

                return nome;
            }
        }
    }
    raiz.filho[i] = remover(raiz.filho[i], ch, t);
    free(irmao);
    free(no);
    return nome;
}

char* retira(char* nome, int k, int t) {
    FILE* arv = fopen(nome, "rb");
    if (!arv || !Busca(nome, k)) {
        fclose(arv);
        return nome;
    }
    fclose(arv);
    return remover(nome, k, t);
}

int main(int argc, char *argv[]) {
    char *resp = "no.dat";
    int o = 0, op, num;
    do {
        printf("Escolha uma acao: \n");
        printf("1- Inserir\n");
        printf("2-Remover\n");
        printf("3-Imprimir\n");
        printf("0-Sair\n");
        scanf("%d", &op);
        switch (op) {
            case 1:
                printf("\nDigite o numero: ");
                scanf("%d", &num);
                resp = Insere(resp, num, &o, t);
                printf("\nInserido...\n\n");
                break;
            case 2:
                printf("\nDigite o numero: ");
                scanf("%d", &num);
                resp = remover(resp, num, t);
                printf("\nRemovido...\n\n");
                break;
            case 3:
                puts("\n");
                Imprime(resp, t);
                printf("\n");
                break;
            case 0:
                break;
        }
    } while (op != 0);
    return 0;
}
