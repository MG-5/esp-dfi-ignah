import { Component } from '@angular/core';
import { Store } from '@ngrx/store';
import { Observable } from 'rxjs';
import { AdditionalVehicle } from 'src/app/models/additional-vehicles';
import { fetchVehicles, pushVehicles, removeVehicle, selectAdditionalVehicles } from 'src/app/store/mode';
import { VehicleDialogComponent } from './vehicle-dialog/vehicle-dialog.component';
import { MatDialog } from '@angular/material/dialog';
import { AsyncPipe } from '@angular/common';
import { MatDividerModule } from '@angular/material/divider';
import { MatIconModule } from '@angular/material/icon';
import { MatButtonModule } from '@angular/material/button';
import { MatListModule } from '@angular/material/list';

@Component({
    selector: 'app-additional-vehicles',
    templateUrl: './additional-vehicles.component.html',
    styleUrls: ['./additional-vehicles.component.scss'],
    standalone: true,
    imports: [MatListModule, MatButtonModule, MatIconModule, MatDividerModule, AsyncPipe]
})
export class AdditionalVehiclesComponent {

  protected vehicles$: Observable<AdditionalVehicle[]> = this.store.select(selectAdditionalVehicles);

  private suspendFetching = false;

  constructor(private store: Store, private dialog: MatDialog) { }

  ngOnInit(): void {
    this.store.dispatch(fetchVehicles());

    setInterval(() => {
      if (!this.suspendFetching) {
        this.store.dispatch(fetchVehicles());
      }
    }, 15000);
  }

  protected addVehicle(): void {
    this.suspendFetching = true;
    const dialogRef = this.dialog.open(VehicleDialogComponent, { data: { vehicle: null, vehicleIndex: null } });

    dialogRef.afterClosed().subscribe(() => {
      this.store.dispatch(pushVehicles());
      this.suspendFetching = false;
    });
  }

  protected onDeleteVehicle(index: number): void {
    this.store.dispatch(removeVehicle({ vehicleIndex: index }));
    this.store.dispatch(pushVehicles());
  }

  protected onEditVehicle(vehicle: AdditionalVehicle, index: number): void {
    this.suspendFetching = true;
    const dialogRef = this.dialog.open(VehicleDialogComponent, { data: { vehicle: vehicle, vehicleIndex: index } });

    dialogRef.afterClosed().subscribe(() => {
      this.store.dispatch(pushVehicles());
      this.suspendFetching = false;
    });
  }

  protected onRefresh(): void {
    this.store.dispatch(fetchVehicles());
  }
}
