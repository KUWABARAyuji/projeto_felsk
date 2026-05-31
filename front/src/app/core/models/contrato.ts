export type StatusContrato =
  | 'PENDENTE'
  | 'CONFIRMADO'
  | 'CONCLUIDO'
  | 'CANCELADO';

export interface Contrato {
  id: number;
  clienteId: number;
  clienteNome: string;
  trabalhadorId: number;
  trabalhadorNome: string;
  habilidade: string;
  data: string;
  horas: number;
  valorTotal: number;
  descricao: string;
  status: StatusContrato;
}

export interface CriarContratoRequest {
  idCliente: number;
  idTrabalhador: number;
  habilidade: string;
  data: string;
  horas: number;
  descricao: string;
}

export interface ConcluirContratoRequest {
  avaliacao: number;
}