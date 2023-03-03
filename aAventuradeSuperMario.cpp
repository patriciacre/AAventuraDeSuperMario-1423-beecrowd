#include <iostream>
#include <vector>
#include <climits> // para utilizar INT_MAX e representar a distância infinita

using namespace std;
enum Tipo {VILA, CASTELO};

class caminho { // classe para armazenar o custo e os vertices inclusos nos subcaminhos em que posso usar a bota
    public:
        caminho();
        virtual ~caminho() {}
        void setCusto(int c) {this->custo = c;}
        int getCusto() const {return this->custo;}
        void setVert(int i) {this->vert.push_back(i);}
        int getVert(int i) const {return this->vert[i];}
        int vertNoCaminho() const {return this->vert.size();}
        void limparCaminho() {this->custo = 0; this->vert = {};}
    protected:
        int custo;
        vector<int> vert; // índices dos vértices no caminho
};

caminho::caminho(){
    this->custo = 0;
    this->vert = {};
}

class vertice{
    public:
        vertice();
        virtual ~vertice() {}
        int getPi() const {return this->pi;}
        void setPi(int p) {this->pi = p;}
        int getD() const {return this->d;}
        void setD(int dist) {this->d = dist;}
        void setTipo(Tipo t) {this->tipo = t;};
        Tipo getTipo() const {return this->tipo;}
    protected:
        int pi; // predecessor
        int d; // distancia
        Tipo tipo; // se é Vila ou Castelo
};

vertice::vertice(){
    this->pi = -1;
    d = 0;
    tipo = {};
}

class grafo{
    public:
        grafo(int A, int B, vector<vector<int>> &adj, vector<vector<int>> &pesos, int L, int K);
        virtual ~grafo() {}
        void initialize_single_source(int s);
        void relax(int o, int v);
        bool bellman_ford(int s);
        void setAdj(vector<vector<int>> &adj) {this->Adj = adj;}
        vector<vector<int>> getAdj() const {return this->Adj;}
        void setW(vector<vector<int>> &w) {this->w = w;}
        vector<vector<int>> getW() const {return this->w;}
        void setU(vector<vertice> &v) {this->u = v;}
        vector<vertice> getU() const {return this->u;}
        int getTime() const {return this->time;};
        void setTime(int t) {this->time = t;}
        int getL() const {return this->L;}
        void setL(int l) {this->L = l;}
        int getK() const {return this->K;}
        void setK(int k) {this->K = k;}
        int getnumV() const {return this->numV;}
        void setnumV(int n) {this->numV = n;}
        void usaBota();
    private:
        vector<vector<int>> Adj; // matriz de adjacência
        vector<vector<int>> w;  // matriz de pesos
        vector<vertice> u; // vetor de vértices que compõe o grafo
        int time; // tempo que será gasto pelo Super Mario no final
        int L; // distância máxima que posso usar a bota
        int K; // nº de vezes q posso usar a bota
        int numV;
};

grafo::grafo(int A, int B, vector<vector<int>> &adj, vector<vector<int>> &pesos, int L, int K){
    u.resize(A+B);
    setnumV(A+B);
    for(int i = 0; i<A; i++){ // inicialmente já marco quem é Vila
       u[i].setTipo(VILA);
    }
    for (int i = A; i<A+B; i++){ // e quem é Castelo
        u[i].setTipo(CASTELO);
    }
    this->Adj = adj;
    this->w = pesos;
    this->time = 0;
    this->L = L;
    this->K = K;
}

void grafo::initialize_single_source(int s){
    for (unsigned long int i = 0; i<u.size();i++){
        this->u[i].setD(INT_MAX); // representa a distância infinita
        this->u[i].setPi(-1);
    }
    this->u[s-1].setD(0);
}
            // recebe o índice dos vértices para fazer o relaxamento
void grafo::relax(int o, int v){
    if ((u[o-1].getD() != INT_MAX) && u[v-1].getD() > (u[o-1].getD() + w[o-1][v-1])){
        u[v-1].setD(u[o-1].getD() + w[o-1][v-1]);
        u[v-1].setPi(o);
    }
}
// para saber quando usar a Bota:
void grafo::usaBota(){
    caminho aux;
    aux.setVert(-1);
    int recomeco = 0;
    vector<caminho> melhorop = {}; //vetor para armazenar as melhores opções de caminho
    bool flag1 = false; // para saber se tem castelos no caminho
    bool flag2 = false; // para saber se terminei de calcular um possível caminho
    int i = 0; // vou percorrer os vértices (Vilas e Castelos) a partir do primeiro, que é o destino
    while (u[i].getPi() != -1){ // encontrando seu predecessor até chegar na origem que é de onde Mario saiu com a Princesa
        int predecessor = u[i].getPi();
        if (w[i][predecessor-1]+aux.getCusto() <= L && flag1 != true){ // verifico se o custo esta dentro da capacidade da bota
            if (aux.getCusto() == 0){
                recomeco = u[i].getPi()-1; // indica que vou iniciar a contagem de um possível caminho para armazenar
            }
            if (u[i].getTipo() == CASTELO && aux.getCusto() != 0){ // caso em que devo parar de usar a bota (não pode ter castelo no meio do caminho)
                flag1 = true;
            } else { // caso não haja castelo do meio do caminho posso continuar somando a distância
                aux.setCusto(aux.getCusto()+w[i][predecessor-1]);
                if (aux.getVert(aux.vertNoCaminho()-1) != i+1) // utilizado para setar o primeiro vértice do caminho
                    aux.setVert(i+1);
                aux.setVert(u[i].getPi()); // registra que o predecessor faz parte do caminho
                if (u[i].getPi() == getnumV() && flag1!=true){ // se eu chegar ao último vértice, que é um castelo
                    melhorop.push_back(aux); // salvo a opção de caminho
                    break;
                }
            }
        } else { // caso não tenha castelos no caminho
            if (flag1!=true){ // o if a seguir testa se o ultimo vertice no caminho da ultima opção é igual ao vertice de aux na pos 1
            // significa que dois caminhos estão contendo o mesmo vertice, sendo que em um caminho o vertice está no meio, assim devo pegar o melhor
                if (melhorop.size() > 0 && melhorop[melhorop.size()-1].getVert(melhorop[melhorop.size()-1].vertNoCaminho()-1) == aux.getVert(1)) {
                    if (melhorop[melhorop.size()-1].getCusto() < aux.getCusto()){ // se o custo de aux for maior irei substituí-lo
                        melhorop[melhorop.size()-1] = aux;
                    }
                } else {
                    melhorop.push_back(aux); //caso contrário apenas isiro aux no final
                }
            }
            aux.limparCaminho(); // reinicia as variáveis auxiliares
            flag1 = false;
            flag2 = true;
        }
        if (flag2 == true){ // reinicia as variáveis auxiliares se já terminei de calcular um possivel caminho
            i = recomeco;
            flag2 = false;
        } else { // se não terminei apenas pego o predecessor
            i = u[i].getPi()-1; // para analisar o predecessor;
        }
    }
    int dTotal = 0;
    for (unsigned long int i=0; i<u.size()-1; i++){ // calculo a distância do caminho total
        dTotal += w[i][u[i].getPi()-1];
    }
    int tempoComBota = 0; int aux2 = 0;
    this->time = dTotal;
    for (int i = 0; i<K; i++){ // percorre o vetor de melhores opções e pego os K caminhos que posso usar a bota
        for (int i = 0; i<melhorop.size();i++){
            if(melhorop[i].getCusto() > tempoComBota){
                tempoComBota = melhorop[i].getCusto();
                aux2 = i;
            }
        }
        this->time -= tempoComBota; // decremento do tempo total o tempo que bota reduziu a zero
        tempoComBota = 0; // reinicio as variáveis auxiliares
        melhorop[aux2].setCusto(0);
    }
}

bool grafo::bellman_ford(int s){
    initialize_single_source(s);
    for (unsigned long int a = 0; a < u.size()-1; a++){ // itera |V|-1 vezes
         for (unsigned long int i = 0; i < u.size(); i++){ // linha da matriz de adjacência
            for (unsigned long int j = 0; j < u.size(); j++){ // coluna da matriz de adjacência
                if (Adj[i][j] == 1){ // se existir aresta entre os vértices
                    relax(i+1, j+1); // faz o relaxamento
                }
            }
        }
    } // testar o relaxamento:
    for (unsigned long int i = 0; i < u.size(); i++){
        for (unsigned long int j = 0; j < u.size(); j++){
            if (Adj[i][j] == 1){ // se existir aresta entre os vértices
                if (u[j].getD() > (u[i].getD() + w[i][j])){ // se reduzir a estimativa
                    return false;           // indica que falhou
                }
            }
        }
    }
    usaBota();
    return true;
}

int main() {
	int T, A, B, M, L, K, Xi, Yi, Li;
	cin >> T;
    for (int i=0; i<T; i++){ // lendo os testes
        cin >> A >> B >> M >> L >> K;
        vector<vector<int>> ad(A+B, vector<int> (A+B, 0)); // matriz de adjacência
        vector<vector<int>> p(A+B, vector<int> (A+B, 0)); // matriz de pesos
        for (int l = 0; l < A+B; l++){ // preencho a matriz de adjacência auxiliar com zeros
            for (int c = 0; c < A+B; c++){
                ad[l][c] = 0;
            }
        }
        for(int i = 0; i < M; i++){ // lendo as arestas (ruas)
            cin >> Xi >> Yi >> Li; // aresta de Xi para Yi e Li é a distância
            ad[Xi-1][Yi-1] = 1; // grafo direcional, posso ir de de Xi para Yi e o contrário tb
            ad[Yi-1][Xi-1] = 1;
            p[Xi-1][Yi-1] = Li;
            p[Yi-1][Xi-1] = Li;
        }
        grafo g(A, B, ad, p, L, K); // entrego o nº de vértices (vilas e castelos), matriz de adjacência, pesos, distância q posso usar a bota e K vezes q posso usá-la
        g.bellman_ford(A+B); // chamo a busca começando pelo último Castelo
        cout << g.getTime() << endl; // mostro o resultado
    }
    return 0;
}
