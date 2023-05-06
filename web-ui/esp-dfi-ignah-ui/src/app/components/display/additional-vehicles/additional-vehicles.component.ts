import { Component } from '@angular/core';
import { Store } from '@ngrx/store';
import { Observable } from 'rxjs';
import { AdditionalVehicle } from 'src/app/models/additional-vehicles';
import { fetchVehicles, selectAdditionalVehicles } from 'src/app/store/mode';

@Component({
  selector: 'app-additional-vehicles',
  templateUrl: './additional-vehicles.component.html',
  styleUrls: ['./additional-vehicles.component.scss']
})
export class AdditionalVehiclesComponent {

  vehicles$: Observable<AdditionalVehicle[]> = this.store.select(selectAdditionalVehicles);

  constructor(private store: Store) {}

  ngOnInit(): void {
    this.store.dispatch(fetchVehicles());
  }
}
