import { Component } from '@angular/core';
import { RouterLink } from '@angular/router';

@Component({
  selector: 'app-home',
  imports: [RouterLink],
  templateUrl: './home.html',
  styleUrl: './home.scss'
})
export class Home {
  servicos = [
    'Instalação elétrica',
    'Reparos hidráulicos',
    'Montagem de móveis',
    'Instalação de prateleiras',
    'Troca de fechaduras',
    'Pequenos reparos residenciais'
  ];

  passos = [
    {
      numero: '01',
      titulo: 'Cliente solicita',
      texto: 'O cliente informa qual serviço precisa, data, horas e descrição.'
    },
    {
      numero: '02',
      titulo: 'Profissional é escolhido',
      texto: 'O sistema usa os trabalhadores cadastrados no backend C++.'
    },
    {
      numero: '03',
      titulo: 'Contrato é criado',
      texto: 'O contrato começa como PENDENTE, igual no seu backend.'
    },
    {
      numero: '04',
      titulo: 'Serviço é finalizado',
      texto: 'Depois ele pode ser confirmado, concluído, avaliado ou cancelado.'
    }
  ];
}