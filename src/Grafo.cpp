#include "Grafo.hpp"
#include <iomanip>
#include <algorithm>


#define ERR_EMPTY_CANDIDATES             1
#define ERR_VERTEX_NON_EXIST             2
#define ERR_BIG_DISTANCE                 3
#define ERR_FULL_TRIPS                   4
#define ERR_NOT_VIABLE                   5

#define LIMITE_BL 0.001

Grafo::Grafo(std::string graphName, size_t numeroDeVertices, size_t numeroDeHoteis, size_t numeroDeTrips, size_t tMax):
    _graphName(graphName),
    _numeroHoteis(numeroDeHoteis),
    _numeroTrips(numeroDeTrips),
    _tMax(tMax)
{
    this->listaVertices.reserve(numeroDeVertices);
    this->listaVariancias.reserve(numeroDeVertices);
    this->listaTamanhoTrips.reserve(numeroDeTrips);
    this->listaTamanhoBestTrips.reserve(numeroDeTrips);
    this->listaCheckTrips.reserve(numeroDeTrips);
    this->listaTamanhoMaxTrips.reserve(numeroDeTrips);
}

void printCustomHeader(const std::string& customText) {
    std::cout << std::endl;
    std::cout << "___________________________________________________________" << std::endl;
    std::cout << std::endl;
    std::cout << customText << std::endl;
    std::cout << "___________________________________________________________" << std::endl;
    std::cout << std::endl;
}


/*-----------------------------------------------------------------------------------------------------*/
/*-----------Sets/Gets---------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------*/

void Grafo::setaScoreSol(double score){
    this->_scoreSol = score;
}
void Grafo::setaCustoSol(double custo){
    this->_custoSol = custo;
}
void Grafo::setaListaSol(listavertices_t listaSolucaoNova){
    this->listaSolucao = listaSolucaoNova;
}
void Grafo::setaTamMaxTrips(listatour_t tamTrips) {
    this->listaTamanhoMaxTrips = tamTrips;
}
void Grafo::setaTamTrips(listatour_t tamTrips) {
    this->listaTamanhoTrips = tamTrips;
}
void Grafo::setaTamBestTrips(listatour_t tamTrips) {
    this->listaTamanhoBestTrips = tamTrips;
}
void Grafo::setaCheckTrips(listatour_t tamTrips) {
    this->listaCheckTrips = tamTrips;
}

void Grafo::atualizaIdTrips(Vertice& v, size_t t, size_t t2) {
    v.updateTripId(t, t2);
}

void Grafo::setaVariancias(listavariancias_t variancias) {
    this->listaVariancias = variancias;
}

void Grafo::setaMatrizDists() {
    this->matrizDist = calculaMatrizDistancias();
}

double Grafo::getCustoTour() {

    double tamanhoTour = 0;
    for (double tamanhoTrip : this->listaTamanhoTrips) {
        tamanhoTour += tamanhoTrip;
    }
    return tamanhoTour;
}

int Grafo::getVerticeIndex(listavertices_t verticeVector, Vertice v)
{
    int index = -1;
    auto it = find(verticeVector.begin(), verticeVector.end(), v);
    if (it != verticeVector.end())
    {
        index = std::distance(verticeVector.begin(), it);
        std::cerr << "O elemento " << v.id() << " foi encontrado no index " << index << std::endl;

    }
    else {
        std::cerr << "O elemento " << v.id() << " não foi encontrado " << std::endl;
    }
    return index;
}



/*-----------------------------------------------------------------------------------------------------*/
/*-----------------------------------------Viabilidade-------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------*/



void Grafo::atualizaTamanhoTripT(listaids_t verticesQuebrados, size_t t){

    double novoCustoTrip = this->listaTamanhoTrips[t - 1] + refazCalculoAresta(verticesQuebrados);
    this->listaTamanhoTrips[t-1] = novoCustoTrip;
}
void Grafo::atualizaTamanhoTripTRemocao(Vertice verticeArrancado, size_t t){

    listaids_t verticesQuebrados;
    verticesQuebrados.push_back(verticeArrancado.id()-1);
    verticesQuebrados.push_back(verticeArrancado.id()+1);
    verticesQuebrados.push_back(verticeArrancado.id());

    double novoCustoTrip = this->listaTamanhoTrips[t - 1] + refazCalculoArestaRemocao(verticesQuebrados);
    this->listaTamanhoTrips[t-1] = novoCustoTrip;
}
double Grafo::refazCalculoAresta(listaids_t verticesQuebrados){
    //0 - entre0 | 1 - entre1 | 2 - verticeNovo
    double refatorado = 0;
    size_t v_origem = verticesQuebrados[0];
    size_t v_destino = verticesQuebrados[1];
    size_t v_novo = verticesQuebrados[2];

    double custoOriginalAresta = matrizDist[v_origem][v_destino];
    double custoArestasNovas = matrizDist[v_origem][v_novo] + matrizDist[v_novo][v_destino];

    refatorado = custoArestasNovas - custoOriginalAresta;
    return refatorado;
}
double Grafo::refazCalculoArestaRemocao(listaids_t verticesQuebrados){
    //0 - entre0 | 1 - entre1 | 2 - verticeVelho
    double refatorado = 0;
    size_t v_origem = verticesQuebrados[0];
    size_t v_destino = verticesQuebrados[1];
    size_t v_novo = verticesQuebrados[2];

    double custoArestasOriginais = matrizDist[v_origem][v_novo] + matrizDist[v_novo][v_destino];
    double custoArestasNovas = matrizDist[v_origem][v_destino];

    refatorado = custoArestasNovas - custoArestasOriginais;
    return refatorado;
}

bool Grafo::insercaoViavel(listaids_t verticesQuebrados, size_t t){
    double diferenca = refazCalculoAresta(verticesQuebrados);
    double novoCustoTrip = this->listaTamanhoTrips[t - 1] + diferenca;
    double novoCustoTour = getCustoTour() + diferenca;
    if(novoCustoTrip < this->listaTamanhoMaxTrips[t - 1] && novoCustoTour <= tamanhoMaximo() && !this->listaCheckTrips[t-1]){
        return true;
    }
    return false;
}

bool Grafo::removeViavel(Vertice verticeArrancado, size_t t){

    listaids_t verticesQuebrados;
    verticesQuebrados.push_back(verticeArrancado.id()-1);
    verticesQuebrados.push_back(verticeArrancado.id()+1);
    verticesQuebrados.push_back(verticeArrancado.id());

    double diferenca = refazCalculoArestaRemocao(verticesQuebrados);
    double novoCustoTrip = this->listaTamanhoTrips[t - 1] + diferenca;
    double novoCustoTour = getCustoTour() + diferenca;
    if(novoCustoTrip > 0 && novoCustoTour >= 0){
        return true;
    }
    return false;
}


/*-----------------------------------------------------------------------------------------------------*/
/*--------------------------------------------Utils----------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------*/



double Grafo::distanciaEuclidiana(Vertice a, Vertice b)
{
    return sqrt(pow((a.x() - b.x()), 2) + pow((a.y() - b.y()), 2));
}

double Grafo::calculaVariancia(double media, double *distancias)
{
    double variancia = 0;
    for(size_t k = 0; k < numeroDeVertices(); k++){
        variancia += pow((distancias[k]-media), 2);
    }
    variancia = variancia/numeroDeVertices();
    return variancia;
}

double** Grafo::calculaMatrizDistancias() {

    double **matrizDistancias = new double *[numeroDeVertices()];
    listavariancias_t variancias;

    for (size_t i = 0; i < numeroDeVertices(); i++) {
        matrizDistancias[i] = new double[numeroDeVertices()];
        double mediaAritmetica = 0;

        for (size_t j = 0; j < numeroDeVertices(); j++) {
            matrizDistancias[i][j] = distanciaEuclidiana(this->getVerticeById(i), this->getVerticeById(j));
            mediaAritmetica += matrizDistancias[i][j];
        }
        mediaAritmetica = mediaAritmetica/numeroDeVertices();
        variancias.push_back(calculaVariancia(mediaAritmetica, matrizDistancias[i]));
    }
    setaVariancias(variancias);
    return matrizDistancias;
}

Vertice Grafo::maiorScore(listavertices_t vertices) {
  Vertice maior = vertices[0];
  for (size_t i = 1; i < vertices.size(); i++) {
    if (vertices[i].score() > maior.score()) {
      maior = vertices[i];
    }
  }
  return maior;
}




/*-----------------------------------------------------------------------------------------------------*/
/*-----------------------------------------Quick-Sort--------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------*/




size_t Grafo::particionamento(listavertices_t& listaOrdenada, size_t p, size_t q, size_t idOrigem)
{
    size_t i = p - 1, j = q;
    double v = matrizDist[idOrigem][listaOrdenada[q].id()];

    while(1)
    {
        while (++i < listaOrdenada.size() && matrizDist[idOrigem][listaOrdenada[i].id()] < v)
        {
            if (idOrigem == listaOrdenada[i].id())
                continue;
        }
        while (v < matrizDist[idOrigem][listaOrdenada[--j].id()])
        {
            if (j == p)
                break;
        }
        if (i >= j)
            break;
        std::swap(listaOrdenada[i], listaOrdenada[j]);
    }
    std::swap(listaOrdenada[i], listaOrdenada[q]);
    return i;
}
void Grafo::auxQuickSort(listavertices_t& listaOrdenada, size_t p, size_t q, size_t idOrigem)
{
    if (p < q)
    {
        size_t j = particionamento(listaOrdenada, p, q, idOrigem);
        if(j != 0)
            auxQuickSort(listaOrdenada, p, j - 1, idOrigem);
        auxQuickSort(listaOrdenada, j + 1, q, idOrigem);
    }
}
listavertices_t Grafo::quickSort(size_t idOrigem, listavertices_t clientesCandidatos)
{
    listavertices_t listaOrdenada = clientesCandidatos;
    auxQuickSort(listaOrdenada, 0, clientesCandidatos.size()-1, idOrigem);

    return listaOrdenada;
}



/*-----------------------------------------------------------------------------------------------------*/
/*-----------------------------------------CONSTRUTIVO-------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------*/



/*--------------------------------*/
/*---ANALISA VIABILIDADE HOTEIS---*/
/*--------------------------------*/

listavertices_t Grafo::selecionaHoteisViaveis(listavertices_t hoteis, Vertice hotelAnterior, size_t t){

    listavertices_t viaveis;

    for (size_t k = 2; k < hoteis.size(); k++){

        /*--------------------------------*/
        /*-------INSTANCIAS GRANDES-------*/
        /*--------------------------------*/

        if(hoteis.size() >= 10){
            if(k!=hotelAnterior.id()){
                if(matrizDist[hotelAnterior.id()][hoteis[k].id()] < this->listaTamanhoMaxTrips[t]){
                    if(t == numeroDeTrips()-2){
                        if(matrizDist[hoteis[k].id()][1] < this->listaTamanhoMaxTrips[t+1]){
                            viaveis.push_back(hoteis[k]);
                        }
                    }
                    else{ viaveis.push_back(hoteis[k]); }
                }
            }
        }

        /*--------------------------------*/
        /*-------INSTANCIAS PEQUENAS-------*/
        /*--------------------------------*/

        else{
            if(matrizDist[hotelAnterior.id()][hoteis[k].id()] < this->listaTamanhoMaxTrips[t]){
                if(t == numeroDeTrips()-2){
                    if(matrizDist[hoteis[k].id()][1] < this->listaTamanhoMaxTrips[t+1]){
                        viaveis.push_back(hoteis[k]);
                    }
                }
                else{ viaveis.push_back(hoteis[k]); }
            }
        }
    }

    return viaveis;
}

/*--------------------------------*/
/*---SELECIONA HOTEIS DA TOUR-----*/
/*--------------------------------*/

listavertices_t Grafo::selecionaHoteisCandidatos(listavertices_t hoteis) {

    listavertices_t resultado;
    listatour_t distanciasResultado;

    /*--------------------------------*/
    /*----SELECIONA HOTEIS VIAVEIS----*/
    /*--------------------------------*/

    bool randViavel;

    do {
        randViavel = true;

        resultado.push_back(hoteis[0]);

        for (size_t i = 0; i < numeroDeTrips() - 1; i++) {
            listavertices_t hoteisViaveis = selecionaHoteisViaveis(hoteis, resultado[i], i);

            if(hoteisViaveis.size() > 1){
                int j = 1 + rand() % (hoteisViaveis.size() - 1);
                do
                {
                    j = 1 + rand() % (hoteisViaveis.size() - 1);
                } while (j == 1);
                resultado.push_back(hoteisViaveis[j]);
            }
            else if (hoteisViaveis.size() == 1){
                resultado.push_back(hoteisViaveis[0]);
            }
            else{
                randViavel = false;
                resultado.clear();
            }
        }
    } while(!randViavel);

    resultado.push_back(hoteis[1]);

    /*--------------------------------*/
    /*---SETA ID TRIP E DISTANCIAS----*/
    /*--------------------------------*/

    for (size_t k = 0; k <= numeroDeTrips()-1; k++){
        distanciasResultado.push_back(matrizDist[resultado[k].id()][resultado[k+1].id()]);
    }
    for (size_t k = 0; k <= numeroDeTrips(); k++){
        if(k==0){
            atualizaIdTrips(resultado[k], 1, 1);
        }
        else if(k==numeroDeTrips()){
            atualizaIdTrips(resultado[k], numeroDeTrips(), numeroDeTrips());
        }
        else{
            atualizaIdTrips(resultado[k], k, k+1);
        }
    }

    setaTamTrips(distanciasResultado);

    listatour_t CheckTripInicial(numeroDeTrips(), 0);
    setaCheckTrips(CheckTripInicial);

    return resultado;
}

/*--------------------------------*/
/*---SELECIONA MELHOR VÉRTICE-----*/
/*--------------------------------*/

Vertice Grafo::selecionaClienteIdeal(listaids_t insereEntre, listavertices_t clientesCandidatos, size_t t){

    listaids_t verticesQuebrados;
    verticesQuebrados.push_back(insereEntre[0]);
    verticesQuebrados.push_back(insereEntre[1]);

    if (clientesCandidatos.size() == 1){
        verticesQuebrados.push_back(clientesCandidatos[0].id());
        if (insercaoViavel(verticesQuebrados, t))
            return clientesCandidatos[0];
        verticesQuebrados.pop_back();
    }

    Vertice v(0, 0, 0, -1, false);

    listavertices_t possiveis0 = quickSort(insereEntre[0], clientesCandidatos);
    listavertices_t possiveis1 = quickSort(insereEntre[1], clientesCandidatos);
    double menorDist0 = matrizDist[insereEntre[0]][possiveis0[0].id()];
    double menorDist1 = matrizDist[insereEntre[1]][possiveis1[0].id()];
    double minimo = 1;
    size_t limitador = 5;

    /*--------------------------------*/
    /*-------CRITÉRIO SIMPLES---------*/
    /*--------------------------------*/

    if(menorDist0 <= minimo){
        verticesQuebrados.push_back(possiveis0[0].id());
        if (insercaoViavel(verticesQuebrados, t))
            return possiveis0[0];
        verticesQuebrados.pop_back();
    }
    else if(menorDist1 <= minimo){
        verticesQuebrados.push_back(possiveis1[0].id());
        if (insercaoViavel(verticesQuebrados, t))
            return possiveis1[0];
        verticesQuebrados.pop_back();
    }
    else{

        /*--------------------------------*/
        /*------RANK POR DISTÂNCIA--------*/
        /*--------------------------------*/

        size_t i = 0;
        listavertices_t tolerancia0;
        listavariancias_t distancias0;
        do
        {
            verticesQuebrados.push_back(possiveis0[i].id());
            if (!insercaoViavel(verticesQuebrados, t))
                break;
            verticesQuebrados.pop_back();
            distancias0.push_back(matrizDist[insereEntre[0]][possiveis0[i].id()]);
            tolerancia0.push_back(possiveis0[i]);
            i = i + 1;

        } while (i < possiveis0.size() && i <= limitador && distancias0[i] < menorDist0 + listaVariancias[insereEntre[0]]);

        size_t j = 0;
        listavertices_t tolerancia1;
        listavariancias_t distancias1;
        do
        {
            verticesQuebrados.push_back(possiveis1[j].id());
            if (!insercaoViavel(verticesQuebrados, t))
                break;
            verticesQuebrados.pop_back();

            distancias1.push_back(matrizDist[insereEntre[1]][possiveis1[j].id()]);
            tolerancia1.push_back(possiveis1[j]);
            j = j + 1;
        } while (j < possiveis1.size()  && j <= limitador && distancias1[j] < menorDist1 + listaVariancias[insereEntre[1]]);


        /*--------------------------------*/
        /*---------RANK POR SCORE---------*/
        /*--------------------------------*/

        listavertices_t toleranciaCombo;
        for(size_t k = 0; k < limitador; k++){
            if(k < i && k < j){
                if(distancias0[k] <= distancias1[k])
                    toleranciaCombo.push_back(tolerancia0[k]);
                else
                    toleranciaCombo.push_back(tolerancia1[k]);
            }
            else if(k < i && k >= j){
                toleranciaCombo.push_back(tolerancia0[k]);
            }
            else if(k >= i && k < j){
                toleranciaCombo.push_back(tolerancia1[k]);
            }
        }

        if (!toleranciaCombo.empty())
            v = maiorScore(toleranciaCombo);
    }

    return v;
}

/*--------------------------------*/
/*---CONDIÇÃO PARADA INSERÇÃO-----*/
/*--------------------------------*/

bool Grafo::condicoesParadaClientes(int& breakFlag, listavertices_t clientesCandidatos){

    /*--------------------------------*/
    /*-------Candidatos Vazia---------*/
    /*--------------------------------*/

    if(clientesCandidatos.empty()){
        breakFlag = ERR_EMPTY_CANDIDATES;
        std::cerr << "LISTA DE CANDIDATOS VAZIA" << std::endl;
        return true;
    }

    /*--------------------------------*/
    /*-------Todas Trips Cheias-------*/
    /*--------------------------------*/

    size_t cont = 0;
    for(size_t l = 0; l < this->listaCheckTrips.size(); l++){
        if (this->listaCheckTrips[l] == 1){
            cont += 1;
        }
    }
    if (cont == numeroDeTrips()){
        breakFlag = ERR_FULL_TRIPS;
        std::cerr << "TODAS AS TRIPS CHEIAS!" << std::endl;
        return true;
    }

    return false;
}

/*--------------------------------*/
/*--------INSERE VERTICES---------*/
/*--------------------------------*/

listavertices_t Grafo::insereClientes(listavertices_t listaCandidatos, listavertices_t& clientesCandidatos) {

    int breakFlag = 0;
    do
    {
        size_t tripAtual = numeroDeTrips();
        for (int i = listaCandidatos.size() - 1; i > 0; i--) {

            if(condicoesParadaClientes(breakFlag, clientesCandidatos)){break;}
            bool insercaoProibida = false;

            /*--------------------------------*/
            /*-------VERTICES DE ORIGEM-------*/
            /*--------------------------------*/

            //IDs APENAS
            listaids_t idsAtuais;
            idsAtuais.push_back(listaCandidatos.at(i).id());
            idsAtuais.push_back(listaCandidatos.at(i-1).id());
            listaids_t idsVerticesQuebrados = idsAtuais;

            //Vertices com TRIP
            listavertices_t candidatosAtuais;
            candidatosAtuais.push_back(listaCandidatos.at(i));
            candidatosAtuais.push_back(listaCandidatos.at(i-1));
            tripAtual = candidatosAtuais[0].tripId().first;


            /*--------------------------------*/
            /*-----SELEÇÃO NOVO VÉRTICE V-----*/
            /*--------------------------------*/

            if(this->listaCheckTrips[tripAtual-1]){
                continue;
            }

            Vertice v = selecionaClienteIdeal(idsAtuais, clientesCandidatos, tripAtual);

            if(v.id() > numeroDeVertices()){
                breakFlag = ERR_VERTEX_NON_EXIST;
                std::cerr << "ERRO: VERTICE NAO EXISTE" << std::endl;
                break;
            }

            /*--------------------------------*/
            /*------TRATAMENTO INSERÇÃO-------*/
            /*--------------------------------*/

            if (v.id() == 0 && !v.isHotel()){
                insercaoProibida = true;
                this->listaCheckTrips[tripAtual-1] = 1;
            }

            /*--------------------------------*/
            /*------INSERE E ATULIZA IDS------*/
            /*--------------------------------*/

            if(!insercaoProibida){
                idsVerticesQuebrados.push_back(v.id());
                atualizaTamanhoTripT(idsVerticesQuebrados, tripAtual);
                atualizaIdTrips(v, tripAtual, tripAtual);
                listaCandidatos.insert(listaCandidatos.begin() + i, v);

                int posicao = getVerticeIndex(clientesCandidatos, v);
                clientesCandidatos.erase(clientesCandidatos.begin() + posicao);
            }

            // imprimeListaTripTour();
        }

    } while (breakFlag == 0);

    std::cout << "Parou por FLAG: " << breakFlag << std::endl;
    return listaCandidatos;
}

/*--------------------------------*/
/*--------ALGORITMO GULOSO--------*/
/*--------------------------------*/

listavertices_t Grafo::guloso(listavertices_t todosHoteisCandidatos, listavertices_t& todosClientesCandidatos){

    /*--------------------------------*/
    /*-------FASE CONSTRUTIVA---------*/
    /*--------------------------------*/

    printCustomHeader("INICIO ETAPA 1 GULOSO - CONSTRUÇÃO");
    listavertices_t listaCandidatos = selecionaHoteisCandidatos(todosHoteisCandidatos);

    printCustomHeader("FIM ETAPA 1 GULOSO - CONSTRUÇÃO");
    imprimeListaVertices(listaCandidatos);

    /*--------------------------------*/
    /*---------FASE INSERÇÃO----------*/
    /*--------------------------------*/


    printCustomHeader("INICIO ETAPA 2 GULOSO - INSERÇÃO");
    listaCandidatos = insereClientes(listaCandidatos, todosClientesCandidatos);

    printCustomHeader("FIM ETAPA 2 GULOSO - INSERÇÃO");
    imprimeListaVertices(listaCandidatos);

    return listaCandidatos;
}

/*--------------------------------*/
/*--------ALGORITMO RVND----------*/
/*--------------------------------*/

// listavertices_t Grafo::rvnd(listavertices_t listaCandidatos){

// }

/*--------------------------------*/
/*-------BUSCA LOCAL 1 MOV--------*/
/*--------------------------------*/

void Grafo::buscaLocal(std::vector<Vertice>& solucao, listavertices_t& clientesRestantes) {

    /*--------------------------------*/
    /*------PARAMETROS-MOVIMENTOS-----*/
    /*--------------------------------*/

    bool melhoriaEncontrada = true;

    /*--------------------------------*/
    /*------EXECUÇÃO-MOVIMENTOS-------*/
    /*--------------------------------*/

    std::vector<Vertice> novaSolucao = solucao;
    size_t tripAtual = 0;
    while (melhoriaEncontrada) {
        melhoriaEncontrada = false;

        for (size_t i = 0; i < solucao.size()-1; i++) {

            if(novaSolucao[i].isHotel()){
                continue;
            }
            tripAtual = novaSolucao[i].tripId().first;

            /*--------------------------------*/
            /*-------MOVIMENTO-1-REMOVE-------*/
            /*--------------------------------*/
            double r = ((double) rand() / (RAND_MAX)) + 1;
            if(r < 0.5){
                if(removeViavel(novaSolucao[i], novaSolucao[i+1].tripId().first)){
                    std::cout << "Fazendo um REMOVE de: " << novaSolucao[i].toString() << std::endl;

                    atualizaTamanhoTripTRemocao(novaSolucao[i], tripAtual);
                    clientesRestantes.push_back(novaSolucao[i]);
                    novaSolucao.erase(novaSolucao.begin()+i);
                }
            }
        }

        /*--------------------------------*/
        /*--------MOVIMENTO-2-INSERT------*/
        /*--------------------------------*/

        listatour_t CheckTripInicial(numeroDeTrips(), 0);
        setaCheckTrips(CheckTripInicial);
        if(!clientesRestantes.empty()){
            novaSolucao = insereClientes(novaSolucao, clientesRestantes);
        }
        else{
            continue;
        }

        double custoAtual = calculaCustoSolucao(solucao);
        double custoNovo = calculaCustoSolucao(novaSolucao);
        double scoreAtual = calculaScoreSolucao(solucao);
        double scoreNovo = calculaScoreSolucao(novaSolucao);
        double qualidadeAtual = scoreAtual - custoAtual;
        double qualidadeNovo = scoreNovo - custoNovo;

        std::cout << "Qualidade Pre Movimentos " << qualidadeAtual <<  std::endl;
        std::cout << "Qualidade Pos Movimentos " << qualidadeNovo << std::endl;

        if (qualidadeNovo > qualidadeAtual) {
            printCustomHeader("MELHOROU");
            setaTamBestTrips(this->listaTamanhoTrips);
            solucao = novaSolucao;
            melhoriaEncontrada = true;
        }
    }

}

/*--------------------------------*/
/*--------CUSTO DA SOLUÇÃO--------*/
/*--------------------------------*/

double Grafo::calculaCustoSolucao(listavertices_t solucao){
    double custo = 0;
    for (size_t i = 0; i < solucao.size()-1; i++) {
        size_t id1 = solucao[i].id();
        size_t id2 = solucao[i+1].id();
        custo += matrizDist[id1][id2];
    }
    return custo;
}

/*--------------------------------*/
/*--------SCORE DA SOLUÇÃO--------*/
/*--------------------------------*/

double Grafo::calculaScoreSolucao(listavertices_t solucao){
    double score = 0;
    for (size_t i = 0; i < solucao.size(); i++) {
        score += solucao[i].score();
    }
    return score;
}

/*--------------------------------*/
/*--------GERA A SOLUÇÃO----------*/
/*--------------------------------*/

void Grafo::geraSolucao(int flagSolucao, size_t maxIt){

    /*--------------------------------*/
    /*--------FASE PREPARAÇÃO---------*/
    /*--------------------------------*/

    listavertices_t todosHoteisCandidatos(this->listaVertices.begin(), this->listaVertices.begin() + 2 + numeroDeHoteis());

    listavertices_t todosClientesCandidatos(this->listaVertices.begin() + 2 + numeroDeHoteis(), this->listaVertices.end());

    listavertices_t solucao;
    listavertices_t melhorSolucao;
    std::vector<double> custosDasSolucoesBL;
    std::vector<double> scoreDasSolucoesBL;
    std::vector<double> qualidadeDasSolucoesBL;
    double itRodadas=0;

    switch (flagSolucao)
    {
    case 1:

        /*--------------------------------*/
        /*------GERA SOLUÇÃO GULOSA-------*/
        /*--------------------------------*/

        printCustomHeader("INICIO GULOSO");
        solucao = guloso(todosHoteisCandidatos, todosClientesCandidatos);

        printCustomHeader("FIM GULOSO");
        imprimeListaVertices(solucao);
        imprimeListaTripTour();
        std::cout << "CUSTO ENCONTRADO GULOSO: " << calculaCustoSolucao(solucao) << std::endl;
        break;

    case 2:

        /*--------------------------------*/
        /*----GERA-SOLUÇÃO-BUSCA-LOCAL----*/
        /*--------------------------------*/

        /*--------------------------------*/
        /*----------OBTEM-GULOSA----------*/
        /*--------------------------------*/

        printCustomHeader("INICIO GULOSO DA BUSCA LOCAL");
        solucao = guloso(todosHoteisCandidatos, todosClientesCandidatos);
        printCustomHeader("FIM GULOSO DA BUSCA LOCAL");
        imprimeListaVertices(solucao);
        imprimeListaTripTour();

        custosDasSolucoesBL.push_back(calculaCustoSolucao(solucao));
        scoreDasSolucoesBL.push_back(calculaScoreSolucao(solucao));
        qualidadeDasSolucoesBL.push_back(calculaScoreSolucao(solucao) - calculaCustoSolucao(solucao));

        std::cout << "CUSTO ENCONTRADO GULOSO: " << custosDasSolucoesBL[0] << std::endl;
        std::cout << "SCORE ENCONTRADO GULOSO: " << scoreDasSolucoesBL[0] << std::endl;

        setaScoreSol(scoreDasSolucoesBL[0]);
        setaCustoSol(custosDasSolucoesBL[0]);

        melhorSolucao = solucao;

        for(size_t it = 0; it < maxIt; it++){

            printCustomHeader("INICIO DA " + std::to_string(it) + "º INTERAÇÃO");
            itRodadas++;

            /*--------------------------------*/
            /*-----------BUSCA-LOCAL----------*/
            /*--------------------------------*/

            printCustomHeader("INICIO BUSCA LOCAL");
            buscaLocal(melhorSolucao, todosClientesCandidatos);
            printCustomHeader("FIM BUSCA LOCAL");

            /*--------------------------------*/
            /*-----------SCORE-CUSTO----------*/
            /*--------------------------------*/

            custosDasSolucoesBL.push_back(calculaCustoSolucao(solucao));
            scoreDasSolucoesBL.push_back(calculaScoreSolucao(solucao));
            qualidadeDasSolucoesBL.push_back(calculaScoreSolucao(solucao) - calculaCustoSolucao(solucao));

            std::cout << "CUSTO ENCONTRADO BUSCA LOCAL: " << custosDasSolucoesBL[it + 1] << std::endl;
            std::cout << "SCORE ENCONTRADO BUSCA LOCAL: " << scoreDasSolucoesBL[it + 1] << std::endl;

            /*--------------------------------*/
            /*----CRITERIO-ATUALIZA-PARA------*/
            /*--------------------------------*/

            double qualidadeAtual = scoreDasSolucoesBL[it + 1] - custosDasSolucoesBL[it + 1];
            double qualidadeMelhor = getScoreSol() - getCustoSol();
            double diferencaQualidade = qualidadeAtual - qualidadeMelhor;

            std::cout << "QUALIDADE ATUAL: " << qualidadeAtual << std::endl;
            std::cout << "QUALIDADE MELHOR: " << qualidadeMelhor << std::endl;
            std::cout << "DIFERENCA QUALIDADE: " << diferencaQualidade << std::endl;

            if(diferencaQualidade > 0.1){
                melhorSolucao = solucao;
                setaScoreSol(scoreDasSolucoesBL[it + 1]);
                setaCustoSol(custosDasSolucoesBL[it + 1]);
            }
            else{break;}

            printCustomHeader("FIM DA " + std::to_string(it) + "º INTERAÇÃO");
            imprimeListaTripTour();

        }
        printCustomHeader("QUALIDADES BL");
        for(size_t it = 0; it <= itRodadas; it++){
            std::cout << "Qualidade de solução em - " << it << " é: " << qualidadeDasSolucoesBL[it] << std::endl;
        }
        setaListaSol(melhorSolucao);
        imprimeListaVertices(melhorSolucao);
        imprimeListaBestTripTour();
        break;

    default:
        std::cerr << "Selecione um tipo de Algoritmo corretamente" << std::endl;
        break;
    }


}


/*-----------------------------------------------------------------------------------------------------*/
/*---------------------------------------LEITURA ARQUIVO-----------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------*/

std::vector <float> separaLinha(std::string linha){
    std::istringstream stream(linha);
    std::string palavra;
    std::vector <float> numeros;

    while (stream >> palavra) {
        char* end;
        float numero = strtof(palavra.c_str(), &end);

        if (*end == '\0') {
            numeros.push_back(numero);
        } else {
            std::cerr << "Erro ao converter a string em float: " << palavra << std::endl;
        }
    }

    return numeros;
}

Grafo Grafo::lerArquivo(std::istream& arqEntrada, std::string nomeArquivo)
{
    std::string graphName = nomeArquivo;
    size_t nVertices;
    size_t nHoteis;
    size_t nTrips;
    size_t tMax;
    std::vector <float> tamTrips;

    std::string linha;
    std::vector <float> numeros;
    int i = -1;

    do {
        std::getline(arqEntrada, linha);
        if (linha == "---------------------------------------------------") {
            continue;
        }

        i += 1;
        numeros = separaLinha(linha);

        if (i == 0){
            nVertices = numeros.at(0);
            nHoteis = numeros.at(1);
            nTrips = numeros.at(2);
            continue;
        }
        if (i == 1){
            tMax = numeros.at(0);
            continue;
        }
        if (i == 2){
            tamTrips = numeros;
            continue;
        }
        if (i == 3){
            break;
        }
    } while (true);

    Grafo g(graphName, nVertices, nHoteis, nTrips, tMax);

    g.setaTamMaxTrips(tamTrips);

    for (size_t i = 0; i < nVertices; i++) {
        std::string str_linha;

        size_t id = i;
        double x, y, score;
        bool isHotel = false;

        getline(arqEntrada, str_linha);
        sscanf(str_linha.c_str(), "%lf%lf%lf", &x, &y, &score);

        if(i < 2+nHoteis){
           isHotel = true;
        }
        g.listaVertices.push_back(Vertice(id, x, y, score, isHotel));
    }

    g.setaMatrizDists();
    g.imprimeGrafo();

    return g;
}



/*-----------------------------------------------------------------------------------------------------*/
/*-------------------------------------FUNÇÕES DE IMPRESSÃO--------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------*/



void Grafo::imprimeGrafo(){

    std::cout << std::endl;
    std::cout << "Instancia: " << graphName() << std::endl;
    std::cout << std::endl;
    std::cout << "___________________________________________________________" << std::endl;
    std::cout << std::endl;
    std::cout << "N: " << numeroDeVertices() << std::endl;
    std::cout << "H: " << numeroDeHoteis() << std::endl;
    std::cout << "D: " << numeroDeTrips() << std::endl;
    std::cout << "T_Max: " << this->_tMax << std::endl;

    std::cout << "T_d: ";

    for (size_t i = 0; i < numeroDeTrips(); i++) {
        std::cout << this->listaTamanhoTrips[i] << " | ";
    }
    std::cout << std::endl;
    imprimeListaVertices(this->listaVertices);

}

void Grafo::imprimeListaVertices(listavertices_t verticesToPrint){
    std::cout << "___________________________________________________________" << std::endl;
    std::cout << std::endl;
    for (size_t i = 0; i < verticesToPrint.size(); i++) {
        if(verticesToPrint[i].isHotel())
            { std::cout << "H" << verticesToPrint[i].id() << " | ";}
        else{ std::cout << "V" << verticesToPrint[i].id() << " | ";}

        std::cout << verticesToPrint[i].toString();
        if(i != verticesToPrint.size()-1){ std::cout << " | " << matrizDist[verticesToPrint[i].id()][verticesToPrint[i+1].id()]; }
        std::cout << " | " << std::endl;
    }
    std::cout << "___________________________________________________________" << std::endl;
    std::cout << std::endl;
}

void Grafo::imprimeListaTripTour(){
    std::cout << "___________________________________________________________" << std::endl;
    std::cout << std::endl;
    std::cout << "Lista da Tour com " << numeroDeTrips() << " Trips" <<  std::endl;
    std::cout << std::endl;
    for (size_t i = 0; i < numeroDeTrips(); i++) {
        std::cout << "TRIP-" << i+1 << " - " << this->listaTamanhoTrips[i] << " | " << this->listaTamanhoMaxTrips[i] <<  " | " << this->listaCheckTrips[i] << std::endl;
    }
    std::cout << "TOUR" << " - " << getCustoTour() << " | " << tamanhoMaximo() << std::endl;

    std::cout << "___________________________________________________________" << std::endl;
    std::cout << std::endl;
}

void Grafo::imprimeListaBestTripTour(){
    std::cout << "___________________________________________________________" << std::endl;
    std::cout << std::endl;
    std::cout << "Lista da Tour com " << numeroDeTrips() << " Trips" <<  std::endl;
    std::cout << std::endl;
    for (size_t i = 0; i < numeroDeTrips(); i++) {
        std::cout << "TRIP-" << i+1 << " - " << this->listaTamanhoBestTrips[i] << " | " << this->listaTamanhoMaxTrips[i] <<  " | " << this->listaCheckTrips[i] << std::endl;
    }
    std::cout << "TOUR" << " - " << getCustoTour() << " | " << tamanhoMaximo() << std::endl;

    std::cout << "___________________________________________________________" << std::endl;
    std::cout << std::endl;
}

