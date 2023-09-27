import { Component, OnInit } from '@angular/core';
import { Store } from '@ngrx/store';
import { getRunningText, selectRunningText, setRunningText } from 'src/app/store/mode';

@Component({
  selector: 'app-running-text',
  templateUrl: './running-text.component.html',
  styleUrls: ['./running-text.component.scss']
})
export class RunningTextComponent implements OnInit {
  
  speed = 50;

  runningText$ = this.store.select(selectRunningText);

  constructor(private store: Store) { }

  ngOnInit(): void {
    this.store.dispatch(getRunningText());
  }

  saveRunningText(text: string): void {
    this.store.dispatch(setRunningText({runningText: {
      text,
      speed: this.speed
    }}));
  }

}
