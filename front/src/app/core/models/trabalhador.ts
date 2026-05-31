import { Habilidade } from './habilidade';

export interface Trabalhador {
  id: number;
  nome: string;
  telefone: string;
  cpf: string;
  avaliacaoMedia: number;
  totalAvaliacoes: number;
  habilidades: Habilidade[];
}