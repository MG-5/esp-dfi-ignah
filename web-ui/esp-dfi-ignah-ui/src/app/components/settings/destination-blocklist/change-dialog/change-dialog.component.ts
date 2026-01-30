import { Component, HostListener, Inject, OnInit } from '@angular/core';
import { FormBuilder, Validators, FormsModule, ReactiveFormsModule } from '@angular/forms';
import { MAT_DIALOG_DATA, MatDialogRef, MatDialogTitle, MatDialogContent, MatDialogActions, MatDialogClose } from '@angular/material/dialog';
import { Store } from '@ngrx/store';
import { addDestination, updateDestination } from 'src/app/store/mode';
import { MatButtonModule } from '@angular/material/button';
import { MatInputModule } from '@angular/material/input';
import { MatFormFieldModule } from '@angular/material/form-field';

type DestinationAction = 'Add' | 'Edit';

@Component({
  selector: 'app-change-dialog',
  templateUrl: './change-dialog.component.html',
  styleUrls: ['./change-dialog.component.scss'],
  imports: [MatDialogTitle, MatDialogContent, FormsModule, ReactiveFormsModule, MatFormFieldModule, MatInputModule, MatDialogActions, MatButtonModule, MatDialogClose]
})
export class ChangeDialogComponent implements OnInit {

  protected destinationForm = this.formBuilder.group({
    destination: ['', Validators.required]
  });

  protected action: DestinationAction = (this.data.destination != null && this.data.destinationIndex != null) ? 'Edit' : 'Add';

  constructor(@Inject(MAT_DIALOG_DATA) public data: { destination?: string, destinationIndex?: number }, public dialogRef: MatDialogRef<ChangeDialogComponent>, private formBuilder: FormBuilder, private store: Store) { }

  @HostListener('window:keyup.Enter')
  onPressEnter(): void {
    if (this.destinationForm.valid) {
      this.addDestination();
    }
  }

  ngOnInit(): void {
    if (this.data.destination) {
      this.destinationForm.setValue({
        destination: this.data.destination
      });
    }
  }

  addDestination(): void {
    const destination: string = this.destinationForm.value.destination || '';

    this.store.dispatch(addDestination({ destination }));
    this.dialogRef.close();
  }

  editDestination(): void {
    const destination: string = this.destinationForm.value.destination || '';

    this.store.dispatch(updateDestination({ destinationIndex: this.data.destinationIndex as number, destination }));
    this.dialogRef.close();
  }
}
