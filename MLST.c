#include<stdio.h>
#include<stdlib.h>
typedef struct edge{ int u, v; } edge;
typedef enum {false = 0, true = !false} bool; 
//////////////Union-Find Set Data Structure//////////////////////
typedef struct UFSet UFSet;
struct UFSet
{
	int (*GetFather)(UFSet* that, int u);
	int (*Merge)(UFSet* that, int u, int v);
	int set_count;
	int *father;
};
int GetFather(UFSet* that, int u)
{
	if (u == that->father[u]) return u;
	that->father[u] = GetFather(that,that->father[u]);
	return that->father[u];
}
int Merge(UFSet* that, int u, int v)
{
	that->father[GetFather(that,v)] = GetFather(that,u);
	that->set_count--;
	return that->father[u];
}
void UFSet_Init(UFSet* that, int n)
{
	int i;
	that->father = malloc(sizeof(int)*n);
	that->GetFather = GetFather;
	that->Merge = Merge;
	for (i = 0; i < n; i++)
	{
		that->father[i] = i;
	}
	that->set_count = n;
}
void UFSet_DeInit(UFSet* that)
{
	free(that->father);
}
////////////////////////Stack///////////////////////////
typedef struct Stack Stack;
struct Stack
{
	edge *stack;
	int top, size;
	edge (*pop)(Stack *that);
	void (*push)(Stack *that, edge x);
	bool (*empty)(Stack *that);
	bool (*full)(Stack *that);
	void (*clear)(Stack *that);
};

edge pop(Stack *that){return that->stack[--that->top];}
void push(Stack *that, edge x){that->stack[that->top++] = x;}
bool empty(Stack *that){return that->top == 0;}
bool full(Stack *that){return that->top == that->size;}
void clear(Stack *that){that->top = 0;}
void Stack_Init(Stack *that, int n)
{
	that->stack = malloc(sizeof(edge)*n);
	that->top = 0;
	that->size = n;
	that->clear = clear;
	that->empty = empty;
	that->full = full;
	that->pop = pop;
	that->push = push;
}
void Stack_DeInit(Stack *that)
{
	free(that->stack);
}
/////////////////////////Some Ugly Stack//////////////////////////
typedef struct Stack_Int Stack_Int;
struct Stack_Int
{
	int *stack;
	int top, size;
	int (*pop)(Stack_Int *that);
	void (*push)(Stack_Int *that, int x);
	bool (*empty)(Stack_Int *that);
	bool (*full)(Stack_Int *that);
	void (*clear)(Stack_Int *that);
	int (*find)(Stack_Int *that, int x);
};
int pop_int(Stack_Int *that){return that->stack[--that->top];}
void push_int(Stack_Int *that, int x){that->stack[that->top++] = x;}
bool empty_int(Stack_Int *that){return that->top == 0;}
bool full_int(Stack_Int *that){return that->top == that->size;}
void clear_int(Stack_Int *that){that->top = 0;}
int find(Stack_Int *that, int x)
{
	int i;
	for (i = that->top; i >= 0; i--)
	{
		if (that->stack[i] == x) return 1;
	}
	return 0;
}
void Stack_Int_Init(Stack_Int *that, int n)
{
	that->stack = malloc(sizeof(int)*n);
	that->top = 0;
	that->size = n;
	that->clear = clear_int;
	that->empty = empty_int;
	that->find = find;
	that->full = full_int;
	that->pop = pop_int;
	that->push = push_int;
}
void Stack_Int_DeInit(Stack_Int *that)
{
	free(that->stack);
}

/////////////////////Main Algorithm/////////////////////////////
bool* MLST(int **A, int n)
{
	UFSet S;
	Stack F;
	Stack_Int S_prime; //'rolling' stack
	int u,v,u0 = 0,u1 = 0;
	int *d = malloc(sizeof(int)*n), d_prime, i, j;
	bool *Relay = malloc(sizeof(int)*n);
	UFSet_Init(&S, n);
	Stack_Init(&F, n);
	Stack_Int_Init(&S_prime, 2);
	memset(d, 0, sizeof(int)*n);
	memset(Relay, 0, sizeof(bool)*n);

	//Step 1: Find Maxinally Leafy Forest
	for (v = 0; v < n; v++)
	{
		S_prime.clear(&S_prime);
		d_prime = 0;
		for (u = 0; u < n; u++)
		{
			if (A[u][v]>0.5)
			{
				if ((S.GetFather(&S,u) != S.GetFather(&S,v)) && (!S_prime.find(&S_prime,S.GetFather(&S,u))))
				{
					d_prime++;
					if (d[v] + d_prime < 3)
					{
						if (S_prime.empty(&S_prime)) u0 = u; //record first u
						S_prime.push(&S_prime,S.GetFather(&S,u));
						if (S_prime.full(&S_prime)) u1 = u; //record second u
					}
					else //d[v]+d_prime >= 3
					{
						if (!S_prime.empty(&S_prime))
						{
							S.Merge(&S, v, u0);
							d[v]++;
							d[u0]++;
							F.push(&F, (edge){ v,u0 });
							if (S_prime.full(&S_prime))
							{
								S.Merge(&S, v, u1);
								d[v]++;
								d[u1]++;
								F.push(&F, (edge){ v,u1 });
							}
						}
						S_prime.clear(&S_prime);
						S.Merge(&S, v, u);
						d[v]++;
						d[u]++;
						F.push(&F, (edge){ v,u });
					}
				}
			}
		}
	}
	//Step 2: Find (M)ST of Disjoint Trees
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < n; j++)
		{
			if (A[i][j]>0.5 && (S.GetFather(&S,i) != S.GetFather(&S,j)))
			{
				S.Merge(&S, i, j);
				F.push(&F, (edge){ i,j });
				d[i]++;
				d[j]++;
				if (S.set_count == 1) break;
			}
		}
		if (S.set_count == 1) break;
	}

	//umcommit the following to check the selected edges
	//while (!F.empty(&F))
	//{
	//	edge t = F.pop(&F);
	//	printf("%d %d\n",t.u+1,t.v+1);
	//}

    for (int i = 0; i < n; i++) if (d[i]>1) Relay[i] = 1;
    UFSet_DeInit(&S);
    Stack_DeInit(&F);
    Stack_Int_DeInit(&S_prime);
    return Relay;
}

#define _MLST_TEST

#ifdef _MLST_TEST
int main() //Domonstrate usage of the MLST function
{
	int n, tmp, i;
	bool *Relay;
	int **A;
	//read map
	scanf("%d",&n);
	A = malloc(sizeof(int*)*n);
	for (i = 0; i < n; ++i) 
	{
		A[i] = malloc(sizeof(int)*n);
	}
	for (i = 0; i < n; ++i) 
	{
		while (1)
		{
			scanf("%d",&tmp);
			if (!tmp) break;
			A[i][tmp-1] = 1;
			A[tmp-1][i] = 1;
		}
	}
	Relay = MLST(A, n);//<-------------------------------------------------
	for (i = 0; i < n; i++)
	{
		//printf("%d ",Relay[i]);
		if (Relay[i]) printf("%d ", i + 1);
	}
	printf("\n");
}
#endif