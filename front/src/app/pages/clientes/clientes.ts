import { Component, OnInit, signal } from '@angular/core';
import { FormsModule } from '@angular/forms';

import { MaridoAluguelApiService } from '../../core/services/marido-aluguel-api';
import { Cliente } from '../../core/models/cliente';

@Component({
  selector: 'app-clientes',
  imports: [FormsModule],
  templateUrl: './clientes.html',
  styleUrl: './clientes.scss'
})
export class Clientes implements OnInit {
  clientes = signal<Cliente[]>([]);
  carregando = signal(false);
  erro = signal('');
  sucesso = signal('');

  nome = signal('');
  telefone = signal('');
  cpf = signal('');
  endereco = signal('');

  constructor(private readonly api: MaridoAluguelApiService) {}

  ngOnInit(): void {
    this.carregarClientes();
  }

  carregarClientes(): void {
    this.carregando.set(true);
    this.erro.set('');

    this.api.listarClientes().subscribe({
      next: clientes => {
        this.clientes.set(clientes);
        this.carregando.set(false);
      },
      error: () => {
        this.erro.set('Não foi possível carregar os clientes.');
        this.carregando.set(false);
      }
    });
  }

  cadastrarCliente(): void {
    this.erro.set('');
    this.sucesso.set('');

    if (!this.nome()) {
      this.erro.set('Informe o nome do cliente.');
      return;
    }

    this.api.cadastrarCliente({
      nome: this.nome(),
      telefone: this.telefone(),
      cpf: this.cpf(),
      endereco: this.endereco()
    }).subscribe({
      next: cliente => {
        this.sucesso.set(`Cliente ${cliente.nome} cadastrado com sucesso.`);
        this.nome.set('');
        this.telefone.set('');
        this.cpf.set('');
        this.endereco.set('');
        this.carregarClientes();
      },
      error: () => {
        this.erro.set('Não foi possível cadastrar o cliente.');
      }
    });
  }
}