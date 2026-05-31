import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { Observable } from 'rxjs';

import { environment } from '../../../environments/environment';

import {
  Cliente,
  CriarClienteRequest
} from '../models/cliente';

import { Trabalhador } from '../models/trabalhador';

import {
  Contrato,
  CriarContratoRequest,
  ConcluirContratoRequest
} from '../models/contrato';

@Injectable({
  providedIn: 'root'
})
export class MaridoAluguelApiService {
  private readonly apiUrl = environment.apiUrl;

  constructor(private readonly http: HttpClient) {}

  listarClientes(): Observable<Cliente[]> {
    return this.http.get<Cliente[]>(`${this.apiUrl}/clientes`);
  }

  cadastrarCliente(cliente: CriarClienteRequest): Observable<Cliente> {
    return this.http.post<Cliente>(`${this.apiUrl}/clientes`, cliente);
  }

  listarTrabalhadores(): Observable<Trabalhador[]> {
    return this.http.get<Trabalhador[]>(`${this.apiUrl}/trabalhadores`);
  }

  listarTrabalhadoresDisponiveis(data: string): Observable<Trabalhador[]> {
    return this.http.get<Trabalhador[]>(
      `${this.apiUrl}/trabalhadores/disponiveis`,
      {
        params: {
          data
        }
      }
    );
  }

  listarContratos(): Observable<Contrato[]> {
    return this.http.get<Contrato[]>(`${this.apiUrl}/contratos`);
  }

  criarContrato(contrato: CriarContratoRequest): Observable<Contrato> {
    return this.http.post<Contrato>(`${this.apiUrl}/contratos`, contrato);
  }

  confirmarContrato(id: number): Observable<Contrato> {
    return this.http.patch<Contrato>(
      `${this.apiUrl}/contratos/${id}/confirmar`,
      {}
    );
  }

  concluirContrato(
    id: number,
    dados: ConcluirContratoRequest
  ): Observable<Contrato> {
    return this.http.patch<Contrato>(
      `${this.apiUrl}/contratos/${id}/concluir`,
      dados
    );
  }

  cancelarContrato(id: number): Observable<Contrato> {
    return this.http.patch<Contrato>(
      `${this.apiUrl}/contratos/${id}/cancelar`,
      {}
    );
  }
}