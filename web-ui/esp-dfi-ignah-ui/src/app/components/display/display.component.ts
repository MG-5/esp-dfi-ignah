import { Component, OnInit } from '@angular/core';
import { Store } from '@ngrx/store';
import { Mode } from 'src/app/models/mode';
import { getMode, selectMode, setMode } from 'src/app/store/mode';
import { AsyncPipe } from '@angular/common';
import { AdditionalVehiclesComponent } from './additional-vehicles/additional-vehicles.component';
import { FreeTextComponent } from './free-text/free-text.component';
import { RunningTextComponent } from './running-text/running-text.component';
import { MatTabsModule } from '@angular/material/tabs';
import { MatButtonModule } from '@angular/material/button';
import { MatOptionModule } from '@angular/material/core';
import { MatSelectModule } from '@angular/material/select';
import { MatFormFieldModule } from '@angular/material/form-field';

@Component({
    selector: 'app-display',
    templateUrl: './display.component.html',
    styleUrls: ['./display.component.scss'],
    standalone: true,
    imports: [MatFormFieldModule, MatSelectModule, MatOptionModule, MatButtonModule, MatTabsModule, RunningTextComponent, FreeTextComponent, AdditionalVehiclesComponent, AsyncPipe]
})
export class DisplayComponent implements OnInit {

  Mode = Mode;

  mode$ = this.store.select(selectMode);

  constructor(private store: Store) { }

  ngOnInit(): void {
    this.store.dispatch(getMode());
  }

  saveMode(mode: Mode): void {
    this.store.dispatch(setMode({mode}));
  }
}
