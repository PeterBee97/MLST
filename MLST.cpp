#include "mex.h"
#define N 1000
struct edge { int u, v; };
class UFSet
{
public:
	UFSet(int n = N)
	{
		father = new int[n];
		for (int i = 0; i < n; i++)
		{
			father[i] = i;
		}
		set_count = n;
	}
	~UFSet(){delete[] father;}
	int GetFather(int u)
	{
		if (u == father[u]) return u;
		father[u] = GetFather(father[u]);
		return father[u];
	}

	int Merge(int u, int v)
	{
		father[GetFather(v)] = GetFather(u);
		set_count--;
		return father[u];
	}
	int set_count;
private:
	int *father;
};
template<class Element> class Stack
{
public:
	Stack(int n = N)
	{
		stack = new Element[n];
		top = 0;
		size = n;
	}
	~Stack(){delete[] stack;}
	Element pop(){return stack[--top];}
	void push(Element x){stack[top++] = x;}
	bool empty(){return top == 0;}
	bool full(){return top == size;}
	void clear(){top = 0;}
	int find(Element x)
	{
		for (int i = top; i >= 0; i--)
		{
			if (stack[i] == x) return 1;
		}
		return 0;
	}
private:
	Element *stack;
	int top, size;
};
//Input:int A[n][n]
void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[] )
{
	int n;
	//std::cin >> n;
	int d[N] = { 0 }, d_prime;
	//bool A[N][N] = { 0 };
	double *A;
	A = mxGetPr(prhs[0]);
	n = mxGetN(prhs[0]);
	UFSet S(n);
	Stack<edge> F(n);
	Stack<int> S_prime(2); //'rolling' stack
	/*int tmp;
	//read map
	for (int i = 0; i < n; ++i) 
	{
		while (1)
		{
			std::cin >> tmp;
			if (!tmp) break;
			A[i][tmp-1] = true;
			A[tmp-1][i] = true;
		}
	}*/

	//Step 1: Find Maxinally Leafy Forest
	for (int v = 0; v < n; v++)
	{
		S_prime.clear();
		d_prime = 0;
		int u0 = 0, u1 = 0;
		for (int u = 0; u < n; u++)
		{
			if (A[u+n*v]>0.5)
			{
				if ((S.GetFather(u) != S.GetFather(v)) && (!S_prime.find(S.GetFather(u))))
				{
					d_prime++;
					if (d[v] + d_prime < 3)
					{
						if (S_prime.empty()) u0 = u; //record first u
						S_prime.push(S.GetFather(u));
						if (S_prime.full()) u1 = u; //record second u
					}
					else //d[v]+d_prime >= 3
					{
						if (!S_prime.empty())
						{
							S.Merge(v, u0);
							d[v]++;
							d[u0]++;
							F.push({ v,u0 });
							if (S_prime.full())
							{
								S.Merge(v, u1);
								d[v]++;
								d[u1]++;
								F.push({ v,u1 });
							}
						}
						S_prime.clear();
						S.Merge(v, u);
						d[v]++;
						d[u]++;
						F.push({ v,u });
					}
				}
			}
		}
	}
	//Step 2: Find (M)ST of Disjoint Trees
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			if (A[i+n*j] && (S.GetFather(i) != S.GetFather(j)))
			{
				S.Merge(i, j);
				F.push({ i, j });
				d[i]++;
				d[j]++;
				if (S.set_count == 1) break;
			}
		}
		if (S.set_count == 1) break;
	}
	//Output result
	/*
	for (size_t i = 0; i < n; i++)
	{
		std::cout << d[i] << " ";
	}
	std::cout << std::endl;
	for (size_t i = 0; i < n; i++)
	{
		std::cout << S.GetFather(i)+1 << " ";
	}
	std::cout << std::endl;
	
	while (!F.empty())
	{
		edge t = F.pop();
		std::cout << t.u+1 << " " << t.v+1 << "\n";
	}
	*/
	plhs[0] = mxCreateDoubleMatrix(2, n - 1, mxREAL);
	double *FF;
	FF = mxGetPr(plhs[0]);
	int i = 0;
	while (!F.empty())
	{
		edge t = F.pop();
		FF[i++] = t.u;
		FF[i++] = t.v;
	}
    plhs[1] = mxCreateDoubleMatrix(1, n, mxREAL);
    double *Relay;
    Relay = mxGetPr(plhs[1]);
    for (int i = 0; i < n; i++) if (d[i]>1) Relay[i] = 1;
}