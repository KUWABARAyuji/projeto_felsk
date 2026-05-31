import { Routes } from '@angular/router';

import { Home } from './pages/home/home';
import { Orcamento } from './pages/orcamento/orcamento';
import { Clientes } from './pages/clientes/clientes';
import { Trabalhadores } from './pages/trabalhadores/trabalhadores';
import { Contratos } from './pages/contratos/contratos';

export const routes: Routes = [
  {
    path: '',
    component: Home
  },
  {
    path: 'orcamento',
    component: Orcamento
  },
  {
    path: 'clientes',
    component: Clientes
  },
  {
    path: 'trabalhadores',
    component: Trabalhadores
  },
  {
    path: 'contratos',
    component: Contratos
  },
  {
    path: '**',
    redirectTo: ''
  }
];