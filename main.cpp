#include "Data.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include <random>

using namespace std;

typedef struct Solucao
{
    vector<int> sequencia;
    double valorObj;
}Solucao;

struct InsertionInfo
{
    int noInserido;
    int arestaRemovida;
    double custo;
};

double calcularCusto(Data& data, vector<int>& v);
void exibirSolucao(Solucao *s);
vector<int> todosOsNos(Data& data);
void escolher3NosAleatorios(Solucao *s, vector<int> v);
void nosRestantes(Solucao *s, vector<int> v, vector<int> &cl);
Solucao construcao(Data &data);
vector<InsertionInfo> calcularCustoInsercao(Data &data, Solucao *s, vector<int>& cl);
void ordenarEmOrdemCrescente(vector<InsertionInfo> &custoInsercao);
void inserirNaSolucao(Solucao* s, int a, int noInserido);
void removerDeCL(vector<int>& cl, int noInserido);
void buscaLocal(Data &data, Solucao *s);
bool bestImprovementSwap(Data &data, Solucao *s);
bool bestImprovement2Opt(Data &data, Solucao *s);
bool bestImprovementOrOpt(Data &data, Solucao *s, int n);

int main(int argc, char** argv) {

    auto data = Data(argc, argv[1]);
    data.read();


    Solucao solucao_parcial = construcao(data);
    Solucao *s = &solucao_parcial;
    s->valorObj = calcularCusto(data, s->sequencia);
    buscaLocal(data, &solucao_parcial);
    exibirSolucao(&solucao_parcial);
    cout<<calcularCusto(data, s->sequencia)<<endl;

    return 0;
}

void exibirSolucao(Solucao *s)
    {
        for(int i = 0; i < s->sequencia.size() - 1; i++)
        {
            cout << s->sequencia[i] << " -> ";
        }
        cout << s->sequencia.back() << endl<<endl;
    }

double calcularCusto(Data& data, vector<int>& v){

    double custo = 0;
    
    for(int i = 0; i < v.size() - 1; i++){

        custo += data.getDistance(v[i], v[i+1]);
    }

    return custo;
}

vector<int> todosOsNos(Data& data)
{
    vector<int> v;
    for(int i = 0; i < data.getDimension(); i++)
    {
        v.push_back(i+1);
    }
    return v;
}

void escolher3NosAleatorios(Solucao *s, vector<int> v) 
{
    int count;
    for(int i = 0; i <= 2; i++) 
    { 
        int a = rand()%v.size(); 
        count = 0;
        for(int j = 0; j<s->sequencia.size(); j++)
        {
            if(s->sequencia[j]!=v[a])
            {
                count++;
            }
            else continue;
        }
        if (count == s->sequencia.size())
        s->sequencia.insert(s->sequencia.end()-1, v[a]); 
        else i--;
    }
}

void nosRestantes(Solucao *s, vector<int> v, vector<int> &cl) 
{ 
    int count; 
    for(auto i : v) 
    { 
        count = 0; 
        for(auto j : s->sequencia) 
        { 
            if(i != j) 
            { 
                count++; 
            } 
            else break; 
        } 
        if(count == s->sequencia.size()) 
        { 
            cl.push_back(i); 
        } 
        else continue; 
    } 
}

Solucao construcao(Data &data)
{
    Solucao s;
    Solucao* ptr = &s;
    vector<int> v = todosOsNos(data);
    vector<int> cl;
    ptr->sequencia = {1, 1};
    escolher3NosAleatorios(ptr, v); 
    nosRestantes(ptr, v, cl);

    while(!cl.empty())
    {
        vector<InsertionInfo> custoInsercao = calcularCustoInsercao(data, &s, cl);
        ordenarEmOrdemCrescente(custoInsercao);
        double alpha = (double) rand() / RAND_MAX;
        int selecionado = rand() % ((int) ceil(alpha * custoInsercao.size()));
        inserirNaSolucao(&s, custoInsercao[selecionado].arestaRemovida, custoInsercao[selecionado].noInserido);
        removerDeCL(cl, custoInsercao[selecionado].noInserido);
    }
    return s;
    
}

vector<InsertionInfo> calcularCustoInsercao(Data &data, Solucao *s, vector<int> &cl)
{
    vector<InsertionInfo> custoInsercao = vector<InsertionInfo>((s->sequencia.size()-1) * cl.size()); //definindo o tamanho do vetor com o número de possibilidades de adicionar um vértice na solução
    int l = 0;
    for(int a = 0; a < s->sequencia.size()-1; a++)
    {
        int i = s->sequencia[a];
        int j = s->sequencia[a+1];
        for(auto k : cl)
        {
            double delta = data.getDistance(i, k) + data.getDistance(j, k) - data.getDistance(i, j);
            custoInsercao[l].custo = delta;
            custoInsercao[l].noInserido = k;
            custoInsercao[l].arestaRemovida = a;
            l++;
        }
    }
    return custoInsercao;
}

void ordenarEmOrdemCrescente(vector<InsertionInfo> &custoInsercao)
{
    sort(custoInsercao.begin(), custoInsercao.end(), [](const InsertionInfo &a, const InsertionInfo &b) {
        return a.custo < b.custo;
    });
}

void inserirNaSolucao(Solucao* s, int a, int noInserido)
{
    s->sequencia.insert(s->sequencia.begin() + a+1, noInserido);
}

void removerDeCL(vector<int> &cl, int noInserido)
{
    auto it = find(cl.begin(), cl.end(), noInserido);
    if (it != cl.end())
    {
        cl.erase(it);
    }

}

void buscaLocal(Data& data, Solucao *s)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());

    vector<int> nl = {1, 2, 3, 4, 5};
    bool improved = false;
    while(nl.empty() == false)
    {
        std::uniform_int_distribution<int> dist(0, nl.size() - 1);
        int n = dist(gen);
        switch (nl[n])
        {
            case 1:
                improved = bestImprovementSwap(data, s);
                break;
            case 2:
                improved = bestImprovement2Opt(data, s);
                break;
            case 3:
                improved = bestImprovementOrOpt(data, s, 1);
                break;
            case 4:
                improved = bestImprovementOrOpt(data, s, 2);
                break;
            case 5:
                improved = bestImprovementOrOpt(data, s, 3);
                break;
        }
        if (improved)
        {
            nl = {1, 2, 3, 4, 5};
        }
        else
        {
            nl.erase(nl.begin() + n);
        }
        
    }

}

bool bestImprovementSwap(Data &data, Solucao *s)
{
    double delta;
    double bestDelta = 0;
    int best_i, best_j;
    for(int i  = 1; i < s->sequencia.size()-1; i++)
    {
        int vi = s->sequencia[i];
        int vi_next = s->sequencia[i+1];
        int vi_prev = s->sequencia[i-1];
        for(int j = i+1; j<s->sequencia.size()-1; j++)
        {
            int vj = s->sequencia[j];
            int vj_next = s->sequencia[j+1];
            int vj_prev = s->sequencia[j-1];
            if(j == i+1)
            {
                delta = -data.getDistance(vj, vj_next) - data.getDistance(vi, vi_prev) + data.getDistance(vj, vi_prev)+ data.getDistance(vi, vj_next);
            }
            else
                delta = -data.getDistance(vi_prev, vi) -data.getDistance(vi, vi_next) + data.getDistance(vi_prev, vj) + data.getDistance(vj, vi_next) - data.getDistance(vj_prev, vj) - data.getDistance(vj, vj_next) + data.getDistance(vj_prev, vi) + data.getDistance(vi, vj_next);
        
            if (delta < bestDelta)
            {
                bestDelta = delta;
                best_i = i;
                best_j = j;
            }
        }
    }
    if(bestDelta < 0)
    {
        swap(s->sequencia[best_i], s->sequencia[best_j]);
        s->valorObj = s->valorObj + bestDelta;
        return true;
    }
    return false;
}

bool bestImprovement2Opt(Data &data, Solucao *s)
{
    double bestDelta = 0;
    int best_i, best_j;
    for(int i  = 0; i < s->sequencia.size()-1; i++)
    {
        int vi = s->sequencia[i];
        int vi_next = s->sequencia[i+1];

        for(int j = i+1; j<s->sequencia.size()-1; j++)
        {
            int vj = s->sequencia[j];
            int vj_next = s->sequencia[j+1];

            double delta = -data.getDistance(vi, vi_next) -data.getDistance(vj, vj_next) + data.getDistance(vi, vj) + data.getDistance(vi_next, vj_next);
        
            if (delta < bestDelta)
            {
                bestDelta = delta;
                best_i = i;
                best_j = j;
            }
        }
    }
    if(bestDelta < 0)
    {
        vector<int> alterados;
        for(int m = best_i+1; m<= best_j; m++)
        {
            alterados.push_back(s->sequencia[m]);
        }
        for(int i = 0; i<alterados.size()/2;i++)
        {
            int temp = alterados[i];
            alterados[i] = alterados[alterados.size()-1-i];
            alterados[alterados.size()-1-i] = temp;
        }
        for(int m = best_i+1; m<= best_j; m++)
        {
            s->sequencia[m] = alterados[m - (best_i+1)];
        }
        s->valorObj += bestDelta;
        return true;
    }
    return false;
        
    
}
bool bestImprovementOrOpt(Data &data, Solucao *s, int n)
{
    double bestDelta = 0;
    int best_i, best_j;
    for(int i  = 1; i < s->sequencia.size()-1; i++)
    {

        for(int j = 1; j+1+n < s->sequencia.size(); j++)
        {
            if (j > i)
            {
                int vi = s->sequencia[i];
                int vi_next = s->sequencia[i+1];

                int vj = s->sequencia[j];
                int vj_next = s->sequencia[j+1];
                int vj_prox = s->sequencia[j+1+n];
                int vj_prox_prev = s->sequencia[j+n];

                double delta = -data.getDistance(vi, vi_next) - data.getDistance(vj, vj_next) 
                - data.getDistance(vj_prox_prev, vj_prox) + data.getDistance(vi, vj_next) + 
                data.getDistance(vi_next, vj_prox_prev) + data.getDistance(vj, vj_prox);
                
                if (delta < bestDelta)
                {
                    bestDelta = delta;
                    best_i = i;
                    best_j = j;
                }
            }
            else if (j < i)
            {
                int vj = s->sequencia[j];
                int vj_next = s->sequencia[j+1];

                int vi = s->sequencia[i];
                int vi_next = s->sequencia[i+1];
                int vi_prox = s->sequencia[i+1+n];
                int vi_prox_prev = s->sequencia[i+n];

                double delta = -data.getDistance(vj, vj_next) - data.getDistance(vi, vi_next) 
                - data.getDistance(vi_prox_prev, vi_prox) + data.getDistance(vj, vi_next) + 
                data.getDistance(vj_next, vi_prox_prev) + data.getDistance(vi, vi_prox);
                
                if (delta < bestDelta)
                {
                    bestDelta = delta;
                    best_i = i;
                    best_j = j;
                    return true;
                }
                return false;
            }
            
        }
    } 
    if(bestDelta < 0)
    {
        vector<int> nos;
        for(int m = best_j+1; m <= best_j+n; m++)
        {
            nos.push_back(s->sequencia[m]);
        }
        for(int m = 0; m < nos.size(); m++)
        {
            s->sequencia.erase(s->sequencia.begin() + best_j + 1);

        }
        for(int m = 0; m < nos.size(); m++)
        {
            s->sequencia.insert(s->sequencia.begin() + best_i+1+m, nos[m]);
        }
        s->valorObj += bestDelta;
        return true;
    }
    return false;

} 
