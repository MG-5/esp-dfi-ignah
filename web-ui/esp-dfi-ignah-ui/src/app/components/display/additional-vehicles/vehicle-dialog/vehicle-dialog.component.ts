import { Component, HostListener, Inject, OnInit } from '@angular/core';
import { FormBuilder, Validators } from '@angular/forms';
import { MAT_DIALOG_DATA, MatDialogRef } from '@angular/material/dialog';
import { Store } from '@ngrx/store';
import { AdditionalVehicle } from 'src/app/models/additional-vehicles';
import { addVehicle, updateVehicle } from 'src/app/store/mode';

type VehicleAction = 'Add' | 'Edit';

@Component({
  selector: 'app-vehicle-dialog',
  templateUrl: './vehicle-dialog.component.html',
  styleUrls: ['./vehicle-dialog.component.scss']
})
export class VehicleDialogComponent implements OnInit {

  protected vehicleForm = this.formBuilder.group({
    destination: ['', Validators.required],
    departure: ['', Validators.required]
  });

  protected action: VehicleAction = (this.data.vehicle && this.data.vehicleIndex) ? 'Edit' : 'Add';

  constructor(@Inject(MAT_DIALOG_DATA) public data: { vehicle?: AdditionalVehicle, vehicleIndex?: number }, public dialogRef: MatDialogRef<VehicleDialogComponent>, private formBuilder: FormBuilder, private store: Store) { }

  @HostListener('window:keyup.Enter')
  onPressEnter(): void {
    if (this.vehicleForm.valid) {
      this.addVehicle();
    }
  }

  ngOnInit(): void {
    if (this.data.vehicle) {
      this.vehicleForm.setValue({
        destination: this.data.vehicle.destination,
        departure: this.data.vehicle.departure
      });
    }
  }

  addVehicle(): void {
    const vehicle: AdditionalVehicle = {
      destination: this.vehicleForm.value.destination || '',
      departure: this.vehicleForm.value.departure || ''
    };

    this.store.dispatch(addVehicle({ vehicle }));
    this.dialogRef.close();
  }

  editVehicle(): void {
    const vehicle: AdditionalVehicle = {
      destination: this.vehicleForm.value.destination || '',
      departure: this.vehicleForm.value.departure || ''
    };

    this.store.dispatch(updateVehicle({ vehicleIndex: this.data.vehicleIndex as number, vehicle }));
    this.dialogRef.close();
  }
}
