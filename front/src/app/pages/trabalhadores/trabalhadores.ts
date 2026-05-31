import { Component, OnInit, signal } from '@angular/core';

import { MaridoAluguelApiService } from '../../core/services/marido-aluguel-api';
import { Trabalhador } from '../../core/models/trabalhador';

@Component({
  selector: 'app-trabalhadores',
  imports: [],
  templateUrl: './trabalhadores.html',
  styleUrl: './trabalhadores.scss'
})
export class Trabalhadores implements OnInit {
  trabalhadores = signal<Trabalhador[]>([]);
  carregando = signal(false);
  erro = signal('');

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
        this.erro.set('Não foi possível carregar os profissionais.');
        this.carregando.set(false);
      }
    });
  }
}