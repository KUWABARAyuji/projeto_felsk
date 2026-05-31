export interface Cliente {
  id: number;
  nome: string;
  telefone: string;
  cpf: string;
  endereco: string;
}

export interface CriarClienteRequest {
  nome: string;
  telefone: string;
  cpf: string;
  endereco: string;
}