#ifndef GRAFO_HPP
#define GRAFO_HPP

#include <vector>
#include <set>
#include <istream>
#include <ostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <algorithm>
#include <list>

#include <iostream>

#include "Vertice.hpp"

using listavertices_t = std::vector<Vertice>;
using listavariancias_t = std::vector<double>;
using listatour_t = std::vector<float>;
using listaids_t = std::vector<size_t>;

class Grafo {
    private:
        std::string _graphName;

        size_t _numeroHoteis;
        size_t _numeroTrips;
        size_t _tMax;
        double _scoreSol;
        double _custoSol;
        listavertices_t listaSolucao;
        listavertices_t listaVertices;
        listavariancias_t listaVariancias;
        listatour_t listaCheckTrips;
        listatour_t listaTamanhoTrips;
        listatour_t listaTamanhoBestTrips;
        listatour_t listaTamanhoMaxTrips;
        double** matrizDist;

        //Sets e Checks Trips
        void setaScoreSol(double score);
        void setaCustoSol(double custo);
        void setaListaSol(listavertices_t listaSolucao);
        void setaTamMaxTrips(listatour_t tamTrips);
        void setaTamTrips(listatour_t tamTrips);
        void setaTamBestTrips(listatour_t tamTrips);
        void setaCheckTrips(listatour_t tamTrips);
        void atualizaIdTrips(Vertice& v, size_t t, size_t t2);

        //Calculos Distancia
        void setaVariancias(listavariancias_t variancias);
        void setaMatrizDists();
        int getVerticeIndex(listavertices_t verticeVector, Vertice v);

        //Viabilidade
        void atualizaTamanhoTripT(listaids_t verticesQuebrados, size_t t);
        void atualizaTamanhoTripTRemocao(Vertice verticeArrancado, size_t t);
        double refazCalculoAresta(listaids_t verticesQuebrados);
        double refazCalculoArestaRemocao(listaids_t verticesQuebrados);
        bool insercaoViavel(listaids_t verticesQuebrados, size_t t);
        bool removeViavel(Vertice verticeArrancado, size_t t);

        //Auxiliares
        double distanciaEuclidiana(Vertice a, Vertice b);
        double calculaVariancia(double media, double *distancias);
        double** calculaMatrizDistancias();
        Vertice maiorScore(listavertices_t vertices);

        //Quicksort
        size_t particionamento(listavertices_t& listaOrdenada, size_t p, size_t q, size_t idOrigem);
        void auxQuickSort(listavertices_t& listaOrdenada, size_t p, size_t q, size_t idOrigem);
        listavertices_t quickSort(size_t idOrigem, listavertices_t verticesCandidatos);

        //Auxiliares Solucao
        listavertices_t selecionaHoteisViaveis(listavertices_t hoteis, Vertice hotelAnterior, size_t t);
        listavertices_t selecionaHoteisCandidatos(listavertices_t hoteis);
        Vertice selecionaClienteIdeal(listaids_t insereEntre, listavertices_t clientesCandidatos, size_t t);
        bool condicoesParadaClientes(int& breakFlag, listavertices_t clientesCandidatos);
        listavertices_t insereClientes(listavertices_t listaCandidatos, listavertices_t& clientesCandidatos);


    public:
        Grafo(std::string graphName, size_t numeroDeVertices, size_t numeroDeHoteis, size_t numeroDeTrips, size_t tMax);

        //Solucao
        double calculaCustoSolucao(listavertices_t solucao);
        double calculaScoreSolucao(listavertices_t solucao);
        listavertices_t guloso(listavertices_t todosHoteisCandidatos, listavertices_t& todosVerticesCandidatos);
        void buscaLocal(std::vector<Vertice>& solucao, listavertices_t& clientesRestantes);
        listavertices_t rvnd(listavertices_t listaCandidatos);
        void geraSolucao(int flagSolucao, size_t maxIt);

        //Entrada
        static Grafo lerArquivo(std::istream& arqEntrada, std::string nomeArquivo);

        //Saida
        void imprimeGrafo();
        void imprimeGrafoHelper();
        void imprimeListaVertices(listavertices_t listaVertice);
        void imprimeListaOrdenada(listavertices_t verticesToPrint, size_t idOrigem);
        void imprimeListaTripTour();
        void imprimeListaBestTripTour();

        //Gets
        Vertice& getVerticeById(size_t id){ return this->listaVertices.at(id); }
        size_t numeroDeVertices() const { return this->listaVertices.size(); };
        size_t numeroDeHoteis() const { return this->_numeroHoteis; };
        size_t numeroDeClientes() const { return numeroDeVertices()-numeroDeHoteis()-2; };
        size_t numeroDeTrips() const { return this->_numeroTrips; };
        size_t tamanhoMaximo() const { return this->_tMax; };
        std::string graphName() const { return this->_graphName; };
        double getCustoTour();
        double getScoreSol() const { return this->_scoreSol; };
        double getCustoSol() const { return this->_custoSol; };
        listavertices_t getListaSol() const { return this->listaSolucao; };

};

#endif // GRAFO_HPP