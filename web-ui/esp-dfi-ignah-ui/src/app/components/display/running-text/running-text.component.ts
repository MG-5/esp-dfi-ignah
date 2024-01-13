import { Component, OnInit } from '@angular/core';
import { Store } from '@ngrx/store';
import { getRunningText, selectRunningText, setRunningText } from 'src/app/store/mode';
import { AsyncPipe } from '@angular/common';
import { MatButtonModule } from '@angular/material/button';
import { FormsModule } from '@angular/forms';
import { MatSliderModule } from '@angular/material/slider';
import { MatInputModule } from '@angular/material/input';
import { MatFormFieldModule } from '@angular/material/form-field';
import { map } from 'rxjs/operators';

@Component({
    selector: 'app-running-text',
    templateUrl: './running-text.component.html',
    styleUrls: ['./running-text.component.scss'],
    standalone: true,
    imports: [MatFormFieldModule, MatInputModule, MatSliderModule, FormsModule, MatButtonModule, AsyncPipe]
})
export class RunningTextComponent implements OnInit {

  speed = 35;
  runningText$ = this.store.select(selectRunningText);

  constructor(private store: Store) { }

  ngOnInit(): void {
    this.store.dispatch(getRunningText());

    // Abonniere auf das Observable und aktualisiere die 'speed'-Variable
    this.runningText$.pipe(
      map(runningText => runningText.speed)
    ).subscribe(speed => this.speed = speed);
  }

  saveRunningText(text: string): void {
    this.store.dispatch(setRunningText({runningText: {
      text,
      speed: this.speed
    }}));
  }

}
