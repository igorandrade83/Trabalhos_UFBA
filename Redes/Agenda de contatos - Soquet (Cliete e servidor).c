#include <stdio.h>
#include <ctype.h>
#include <conio.h>
#include <winsock.h>
#include <windows.h>
#include <stdlib.h>
// ********************************* Estrutura *********************************

struct prod
{
	char Nome[50];
	char telefone[12];
	struct prod *prox;
	struct prod *ant;
};

	WSADATA data;
	SOCKET winsock;
	SOCKADDR_IN sock;
	int bytes;


// ***************************** Declaracao das funcoes *****************************

void Menu();
void AbrirArquivo(struct prod **Lista, FILE *arquivo );
void Insere(struct prod **Lista, char *contato, int QuemChama);
void Exibe(struct prod *Lista);
void Apaga(struct prod **Lista);
int Pesquisar(struct prod *Lista, char *procura, int QuemChama);
void SalvaArquivo(struct prod *Lista, FILE *arquivo );
void RecebeAgenda (struct prod **Lista);
void EnviaAgenda (struct prod *Lista);
// ********************************* main *********************************

int main()
{
	Menu();
	system("pause");
	return 0;
}

// ********************************* Funcoes *********************************

void Menu()
{
	char apagar[80];
	char procura[50],salvar;
	char contato[] = "0";
	int QuemChama =1;
	int op;
	FILE *arquivo = NULL;
	struct prod *Lista= NULL;
	AbrirArquivo(&Lista,arquivo);
	while(1)
	{
		system("cls");
		printf("\t\tAgenda\n"
				"\t0 - Abrir Arquivo\n"
				"\t1 - Insere\n"
				"\t2 - Exibe\n"
				"\t3 - Apaga\n"
				"\t4 - Pesquiar\n"
				"\t5 - Salvar\n"
				"\t6 - Upload Contatos - Cliente\n"
				"\t7 - Download Contatos - Server\n"
				"Opcao: ");
		scanf("%d", &op);
		system("cls");

		switch (op)
		{
			case 0:
			{
				AbrirArquivo(&Lista,arquivo);
				break;
			}
			case 1:
			{
				Insere(&Lista,contato,QuemChama );
				break;
			}
			case 2:
			{
				Exibe(Lista);
				break;
			}
			case 3:
			{
				Apaga(&Lista);
				break;
			}
			case 4:
			{
				if(Lista == NULL)
				{
					printf("Lista Vazia!\n");
					system("pause");
				}
				else
				{
					printf("\nInforme contato para pesquisar: ");
					scanf("\n\r%[^\n]", &procura);
					Pesquisar(Lista,procura,1);

				}
				break;
			}
			case 5:
			{
				if(Lista == NULL)
				{
					printf("Lista Vazia, nao ha o que salvar.\n");
					system("pause");
				}
				else
				{
					printf("Ao salvar, tudo que tem no arquivo sera excluido, abra o arquivo antes.\nDeseja Salvar (s/n): ");
					setbuf(stdin,NULL);
					scanf("%c",&salvar);
					if(toupper(salvar) == 'S')
						SalvaArquivo(Lista,arquivo);
				}
				break;
			}
			case 6:
			{
				EnviaAgenda(Lista);
				break;
			}
			case 7:
			{
				RecebeAgenda(&Lista);
				break;
			}
			default:
			{
				break;
			}
		}
	}



	return;
}

void AbrirArquivo(struct prod **Lista, FILE *arquivo )
{
	arquivo = fopen("agenda.txt","r");
	char VerificaArquivoVazio[50];
	struct prod *Novo = NULL;
	struct prod *corre = *Lista;

	fscanf(arquivo,"Nome: %s", &VerificaArquivoVazio);
	if(strlen(VerificaArquivoVazio) == 0)		// Verifica se o arquivo ta vazio, Se conseguir copiar algo, nao ta vazio.
	{
		fclose(arquivo);
		printf("Nao tem arquivo salvo.\n");
		system("pause");
		return;
	}
	rewind(arquivo);			//Volta para o comeco do arquivo.

	while(!feof(arquivo))
	{
		Novo = (struct prod *)malloc(sizeof(struct prod));

		Novo->prox = NULL;
		Novo->ant = NULL;

		do									// O 'do while' serve para verificar se ja existe um contato de mesmo nome na memoria, se ja existir, ele nao copiara do arquivo.
		{printf("teste\n");
			fscanf(arquivo,"Nome: %s\tTelefone: %s\n", &Novo->Nome, &Novo->telefone);
		}while((Pesquisar(*Lista,Novo->Nome,0) == 1) && !feof(arquivo))	;

		if(*Lista == NULL)				// Resolve Primeiro NO.
		{
			*Lista = corre = Novo;		//Lista e corre apontam pro primeiro No.
		}
		else							// Resolve Segundo no em diante.
		{
			if(Pesquisar(*Lista,Novo->Nome,0) == 0)
			{
				while(corre->prox != NULL)
				{
					corre = corre->prox;
				}
				corre->prox = Novo;
				Novo->ant = corre;
				corre = corre->prox;
			}

		}
	}

	fclose(arquivo);
	return;
}


void Insere(struct prod **Lista, char *contato, int QuemChama )
{

/*
- QuemChama serve pra saber se a funcao eh chamada no Menu (1) ou na funcao RecebeAgenda (0),
  dependendo de quem a chamou, se comportara de forma diferente, chamada pelo:
	* menu, ele os dados digitados pelo usuario.
	* RecebeAgenda, recebe os dados socket .
*/
	char recebedados1[50];
	char recebedados2[12];
	char alterar;
	struct prod *Novo = (struct prod *)malloc(sizeof(struct prod));
	struct prod *corre = *Lista;
	int i=0, j=0;

	Novo->prox = NULL;
	Novo->ant = NULL;

	if(QuemChama == 0)		// Resolver qnd recebe dados do cliente.
	{
		while(contato[i] != '#')
		{
			recebedados1[i] = contato[i];
			++i;
		}
		recebedados1[i] = '\0';
		memcpy ( Novo->Nome, recebedados1, strlen(recebedados1)+1 );
		++i;
		while(contato[i] != '\0')
		{
			recebedados2[j] = contato[i];
			++j;
			++i;
		}
		recebedados2[j] = '\0';
		memcpy(Novo->telefone,recebedados2,sizeof(recebedados2)+1);
	}
	else					// Resolver qnd recebe do teclado.
	{
		printf("Nome: ");
		setbuf(stdin,NULL);
		scanf("\n\r%[^\n]", &Novo->Nome);
		printf("Telefone: ");
		setbuf(stdin,NULL);
		scanf("\n\r%[^\n]", &Novo->telefone);
	}


	if(Pesquisar(*Lista,Novo->Nome,0) == 1)			// Se o nome inserido ja existir na lista, entao ele pergunta se deseja alterar os valores;
	{
		printf("\n'%s' ja existe!\ndeseja alterar para os novos valores (s/n)? ");
		setbuf(stdin,NULL);							//Limpa buffer do teclado.
		scanf("%c",&alterar);
		if(toupper(alterar) == 'S')
		{
			while(corre != NULL)
			{
				if(stricmp(corre->Nome,Novo->Nome)== 0)
				{
					memcpy ( corre->telefone, Novo->telefone, strlen(Novo->telefone)+1 );
				}
				corre = corre->prox;
			}
		}
		free(Novo);
	}
	else											// Se nao existir na lista, ele ira inserir no final da lista.
	{
		if(*Lista == NULL)
		{
			*Lista = Novo;
		}
		else
		{
			struct prod *corre = *Lista;
			while(corre->prox != NULL)
			{
				corre = corre->prox;
			}
			corre->prox = Novo;
			Novo->ant = corre;
		}
	}
	return;
}

void Exibe(struct prod *Lista)
{
	struct prod *corre = Lista;

	if(Lista == NULL)
		printf("Lista vazia.\n");
	else
	{
		do
		{
			printf("\nNome: %s", corre->Nome);
			printf("\nTelefone: %s\n", corre->telefone);
			corre = corre->prox;
		}while(corre != NULL);
	}
	system("pause");
	return;
}

void Apaga(struct prod **Lista)
{
/*
- O pq de ta tao confuso:
	* Se so tiver um unico contato na lista, tem que liberar e apontar Lista pra NULL;
	* Se for o primeiro no, tem que apontar 'ant' do segundo no pra NULL e Lista pro segundo;
	* Se for um do meio, tem que apontar o anterior pro posterior e posterior pro anterior (o caso mais comum);
	* Se for o ultimo da lista, tem que apontar o 'prox' do penultimo pra NULL.
*/

	struct prod *corre = *Lista;
	struct prod *aux = *Lista;
	char procura[50];

	if(*Lista == NULL)								// Resolvendo nenhum contato.
	{
		printf("Lista vazia.\n");
		system("pause");
	}
	else
	{
		printf("\nInforme contato para apagar: ");
		scanf("\n\r%[^\n]", &procura);


		if((*Lista)->prox == NULL)				// Resolvendo so 1 contato.
		{
			if(stricmp(corre->Nome,procura)== 0)
			{
				free(corre);
				*Lista = NULL;

			}
		}

		while(corre != NULL)						// Resolvendo todos os outros casos.
		{
			if(stricmp(corre->Nome,procura)== 0)
			{
					if(*Lista == corre)						// Resolvendo quando eh o primeiro da Lista
					{
						*Lista = (*Lista)->prox;
						(*Lista)->ant = NULL;
					}
					else
					{
						aux = corre;
						corre = corre->ant;
						corre->prox = corre->prox->prox;	//Se for o ultimo no, ele nao entra no if debaixo.
						if(aux->prox != NULL)				// Resolvendo quando o no nao eh o ultimo da lista.
						{
							corre = aux->prox;
							corre->ant = corre->ant->ant;
							corre = corre->ant;
						}
						free(aux);
					}
			}
			corre = corre->prox;
		}
	}
	return;
}


int Pesquisar(struct prod *Lista, char *procura, int QuemChama)
{

/*
- QuemChama serve pra saber se a funcao eh chamada no Menu (1) ou na funcao AbrirArquivo (0),
  dependendo de quem a chamou, se comportara de forma diferente, chamada pelo:
	* menu, ele imprime o contato encontrado.
	* AbrirArquivo, a funcao retorna '1' se achou contato ou '0', caso nao.
*/

	struct prod *corre = Lista;
	int Achou=0;				// Se 0, nao achou nome correspondente.

	while(corre != NULL)
	{
		if(stricmp(corre->Nome,procura)== 0)
		{
			Achou = 1;
			if(QuemChama)
			{
				printf("\nNome: %s", corre->Nome);
				printf("\nTelefone: %s\n", corre->telefone);
				system("pause");
			}
		}
		corre = corre->prox;
	}
	if(Achou == 0 && QuemChama == 1)
	{
		printf("Contato nao encontrado.\n");
		system("pause");
	}
	return Achou;
}


void SalvaArquivo(struct prod *Lista, FILE *arquivo )
{
	remove("agenda.txt");				//Limpa o arquivo.
	arquivo = fopen("agenda.txt","w");
	struct prod *corre = Lista;
	if(arquivo == NULL)
	{
		printf("Nao foi possivel criar/abrir o arquivo.\n");
		exit(0);
	}

	if(Lista == NULL)
		printf("Lista vazia.\n");
	else
	{
		while(corre != NULL)
		{
			fprintf(arquivo,"Nome: %s\tTelefone: %s\n", corre->Nome, corre->telefone);
			corre = corre->prox;
		}
	}
	fclose(arquivo);
	return;
}



void RecebeAgenda (struct prod **Lista)	//Server - 7
{
	int i=1;
	char buffer[1024];
	char buffer2[1024];

	struct prod *corre = *Lista;
	if(corre != NULL)
	{
		while(corre->prox!=NULL)
		{
			corre = corre->prox;
		}
	}


	if(WSAStartup(MAKEWORD(1,1),&data)==SOCKET_ERROR)
	{
		printf("Erro ao inicializar o winsock");
		system("pause");
		return;
	}
	if((winsock = socket(AF_INET,SOCK_STREAM,0))==SOCKET_ERROR)
	{
		printf("Erro ao criar socket");
		system("pause");
		return;
	}
	sock.sin_family=AF_INET;
	sock.sin_port=htons(1234);
	if(bind(winsock,(SOCKADDR*)&sock,sizeof(sock))==SOCKET_ERROR)
	{
		printf("Erro utilizar a funcao BIND");
		system("pause");
		return;
	}
	listen(winsock,1);
	while((winsock = accept(winsock,0,0))==SOCKET_ERROR)
	{
		Sleep(1);
		printf("sleep");
	}
	printf("Cliente conectado!\n");
	while(i==1)
	{
		//Sleep(1);
		memset(buffer2,0,1024);
		bytes=recv(winsock,buffer2,1024,0);
		if(bytes==-1)
		{
			printf("Conexão perdida");
			getch();
			return;
		}
		strcat(buffer,"ok");
		send(winsock,buffer,strlen(buffer),0);

		if(buffer2[0] == '1')
		{
			i=0;
		}
		else
			Insere(&*Lista,buffer2,0);

	}
	closesocket(winsock);
	WSACleanup();
	return;

}


// *********************************************************************************
void EnviaAgenda (struct prod *Lista)	//Cliente - 6
{

	char buffer[1024];
	char buffer2[1024];

	struct prod *corre = Lista;

	if(Lista == NULL)
	{
		printf("Sem contatos.\n");
		return;
	}
	if(WSAStartup(MAKEWORD(1,1),&data)==SOCKET_ERROR)
	{
		printf("Erro ao inicializar o winsock");
		system("pause");
		return;
	}
	if((winsock = socket(AF_INET,SOCK_STREAM,0))==SOCKET_ERROR)
	{
		printf("Erro ao criar socket");
		system("pause");
		return;
	}
	sock.sin_family=AF_INET;
	sock.sin_port=htons(1234);
	sock.sin_addr.s_addr=inet_addr("127.0.0.1");
	if(connect(winsock,(SOCKADDR*)&sock,sizeof(sock))==SOCKET_ERROR)
	{
		printf("Erro ao se conectar");
		system("pause");
		return;
	}
	printf("Conectado!\n");

	while(corre != NULL)
	{
		buffer[0] = '\0';
		strcat(buffer,corre->Nome);
		strcat(buffer,"#");
		strcat(buffer,corre->telefone);
		Sleep(1);
		//printf("\n' %s '\n", buffer);
		//strcat(buffer,"\r\n");

		send(winsock,buffer,strlen(buffer),0);
		memset(buffer2,0,1024);
		bytes=recv(winsock,buffer2,1024,0);
		if(bytes==-1)
		{
			printf("Conexão perdida");
			getch();
			return;
		}
		corre = corre->prox;
	}
	buffer[0] = '1';
	buffer[1] = '\0';		// acabar a conexao do outro lado.
	send(winsock,buffer,strlen(buffer),0);

	closesocket(winsock);
	WSACleanup();

	return;
}
