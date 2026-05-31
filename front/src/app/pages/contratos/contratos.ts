import { Component, OnInit, signal } from '@angular/core';

import { MaridoAluguelApiService } from '../../core/services/marido-aluguel-api';
import { Contrato } from '../../core/models/contrato';

@Component({
  selector: 'app-contratos',
  imports: [],
  templateUrl: './contratos.html',
  styleUrl: './contratos.scss'
})
export class Contratos implements OnInit {
  contratos = signal<Contrato[]>([]);
  carregando = signal(false);
  erro = signal('');
  sucesso = signal('');

  constructor(private readonly api: MaridoAluguelApiService) {}

  ngOnInit(): void {
    this.carregarContratos();
  }

  carregarContratos(): void {
    this.carregando.set(true);
    this.erro.set('');

    this.api.listarContratos().subscribe({
      next: contratos => {
        this.contratos.set(contratos);
        this.carregando.set(false);
      },
      error: () => {
        this.erro.set('Não foi possível carregar os contratos.');
        this.carregando.set(false);
      }
    });
  }

  confirmar(id: number): void {
    this.api.confirmarContrato(id).subscribe({
      next: () => {
        this.sucesso.set(`Contrato #${id} confirmado.`);
        this.carregarContratos();
      },
      error: () => {
        this.erro.set('Erro ao confirmar contrato.');
      }
    });
  }

  concluir(id: number): void {
    const avaliacao = Number(prompt('Avaliação do trabalhador de 0 a 5:'));

    if (Number.isNaN(avaliacao) || avaliacao < 0 || avaliacao > 5) {
      this.erro.set('Avaliação inválida.');
      return;
    }

    this.api.concluirContrato(id, { avaliacao }).subscribe({
      next: () => {
        this.sucesso.set(`Contrato #${id} concluído.`);
        this.carregarContratos();
      },
      error: () => {
        this.erro.set('Erro ao concluir contrato.');
      }
    });
  }

  cancelar(id: number): void {
    this.api.cancelarContrato(id).subscribe({
      next: () => {
        this.sucesso.set(`Contrato #${id} cancelado.`);
        this.carregarContratos();
      },
      error: () => {
        this.erro.set('Erro ao cancelar contrato.');
      }
    });
  }
}