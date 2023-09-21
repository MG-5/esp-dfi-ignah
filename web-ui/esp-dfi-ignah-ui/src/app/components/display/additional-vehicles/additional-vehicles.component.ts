import { Component } from '@angular/core';
import { Store } from '@ngrx/store';
import { Observable } from 'rxjs';
import { AdditionalVehicle } from 'src/app/models/additional-vehicles';
import { fetchVehicles, pushVehicles, removeVehicle, selectAdditionalVehicles } from 'src/app/store/mode';
import { VehicleDialogComponent } from './vehicle-dialog/vehicle-dialog.component';
import { MatDialog } from '@angular/material/dialog';

@Component({
  selector: 'app-additional-vehicles',
  templateUrl: './additional-vehicles.component.html',
  styleUrls: ['./additional-vehicles.component.scss']
})
export class AdditionalVehiclesComponent {

  protected vehicles$: Observable<AdditionalVehicle[]> = this.store.select(selectAdditionalVehicles);

  constructor(private store: Store, private dialog: MatDialog) {}

  ngOnInit(): void {
    this.store.dispatch(fetchVehicles());
  }

  protected addVehicle(): void {
    const dialogRef = this.dialog.open(VehicleDialogComponent);
    dialogRef.afterClosed().subscribe(() => this.store.dispatch(pushVehicles()));
  }

  protected onDeleteVehicle(index: number): void {
    this.store.dispatch(removeVehicle({vehicleIndex: index}));
  }
}
