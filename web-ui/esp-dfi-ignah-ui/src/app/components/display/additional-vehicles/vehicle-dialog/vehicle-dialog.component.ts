import { Component } from '@angular/core';
import { FormBuilder, Validators } from '@angular/forms';
import { MatDialogRef } from '@angular/material/dialog';
import { Store } from '@ngrx/store';
import { AdditionalVehicle } from 'src/app/models/additional-vehicles';
import { addVehicle } from 'src/app/store/mode';

@Component({
  selector: 'app-vehicle-dialog',
  templateUrl: './vehicle-dialog.component.html',
  styleUrls: ['./vehicle-dialog.component.scss']
})
export class VehicleDialogComponent {

  form = this.formBuilder.group({
    destination: ['', Validators.required],
    departure: ['', Validators.required]
  });

  constructor(public dialogRef: MatDialogRef<VehicleDialogComponent>, private formBuilder: FormBuilder, private store: Store) {}

  addVehicle(): void {
    const vehicle: AdditionalVehicle = {
      destination: this.form.value.destination || '',
      departure: this.form.value.departure || ''
    };

    this.store.dispatch(addVehicle({ vehicle }));
    this.dialogRef.close();
  }
}
