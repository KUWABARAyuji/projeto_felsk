import { Component, OnInit, signal } from '@angular/core';
import { FormsModule } from '@angular/forms';

import { MaridoAluguelApiService } from '../../core/services/marido-aluguel-api';
import { Trabalhador } from '../../core/models/trabalhador';

@Component({
  selector: 'app-orcamento',
  imports: [FormsModule],
  templateUrl: './orcamento.html',
  styleUrl: './orcamento.scss'
})
export class Orcamento implements OnInit {
  trabalhadores = signal<Trabalhador[]>([]);
  carregando = signal(false);
  erro = signal('');
  sucesso = signal('');

  idCliente = signal<number | null>(null);
  idTrabalhador = signal<number | null>(null);
  habilidade = signal('');
  data = signal('');
  horas = signal(1);
  descricao = signal('');

  constructor(private readonly api: MaridoAluguelApiService) {}

  ngOnInit(): void {
    this.carregarTrabalhadores();
  }

  carregarTrabalhadores(): void {
    this.carregando.set(true);
    this.erro.set('');

    this.api.listarTrabalhadores().subscribe({
      next: trabalhadores => {
        this.trabalhadores.set(trabalhadores);
        this.carregando.set(false);
      },
      error: () => {
        this.erro.set('Backend C++ ainda não respondeu em /trabalhadores.');
        this.carregando.set(false);
      }
    });
  }

  trabalhadorSelecionado(): Trabalhador | undefined {
    return this.trabalhadores().find(
      trabalhador => trabalhador.id === this.idTrabalhador()
    );
  }

  criarContrato(): void {
    this.erro.set('');
    this.sucesso.set('');

    if (
      !this.idCliente() ||
      !this.idTrabalhador() ||
      !this.habilidade() ||
      !this.data() ||
      !this.horas()
    ) {
      this.erro.set('Preencha cliente, trabalhador, habilidade, data e horas.');
      return;
    }

    this.api.criarContrato({
      idCliente: this.idCliente()!,
      idTrabalhador: this.idTrabalhador()!,
      habilidade: this.habilidade(),
      data: this.data(),
      horas: this.horas(),
      descricao: this.descricao()
    }).subscribe({
      next: contrato => {
        this.sucesso.set(`Contrato #${contrato.id} criado com sucesso.`);
      },
      error: () => {
        this.erro.set('Erro ao enviar contrato para o backend C++.');
      }
    });
  }
}