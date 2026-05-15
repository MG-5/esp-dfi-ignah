import { Component } from '@angular/core';
import { Store } from '@ngrx/store';
import { Observable } from 'rxjs';
import { DestinationBlocklist } from '@app/models/destination-blocklist';
import { fetchDestinationBlocklist, pushDestinationBlocklist, removeDestination, selectDestinationBlocklist } from '@app/store/mode';
import { ChangeDialogComponent } from './change-dialog/change-dialog.component';
import { MatDialog } from '@angular/material/dialog';
import { AsyncPipe } from '@angular/common';
import { MatDividerModule } from '@angular/material/divider';
import { MatIconModule } from '@angular/material/icon';
import { MatButtonModule } from '@angular/material/button';
import { MatListModule } from '@angular/material/list';

@Component({
  selector: 'app-destination-blocklist',
  templateUrl: './destination-blocklist.component.html',
  styleUrls: ['./destination-blocklist.component.scss'],
  imports: [MatListModule, MatButtonModule, MatIconModule, MatDividerModule, AsyncPipe]
})
export class DestinationBlocklistComponent {

  protected destinations$: Observable<string[]> = this.store.select(selectDestinationBlocklist);

  private suspendFetching = false;

  constructor(private store: Store, private dialog: MatDialog) { }

  ngOnInit(): void {
    this.store.dispatch(fetchDestinationBlocklist());

    setInterval(() => {
      if (!this.suspendFetching) {
        this.store.dispatch(fetchDestinationBlocklist());
      }
    }, 15000);
  }

  protected addDestination(): void {
    this.suspendFetching = true;
    const dialogRef = this.dialog.open(ChangeDialogComponent, { data: { destination: null, destinationIndex: null } });

    dialogRef.afterClosed().subscribe(() => {
      this.store.dispatch(pushDestinationBlocklist());
      this.suspendFetching = false;
    });
  }

  protected onDeleteDestination(index: number): void {
    this.store.dispatch(removeDestination({ destinationIndex: index }));
    this.store.dispatch(pushDestinationBlocklist());
  }

  protected onEditDestination(destination: string, index: number): void {
    this.suspendFetching = true;
    const dialogRef = this.dialog.open(ChangeDialogComponent, { data: { destination: destination, destinationIndex: index } });

    dialogRef.afterClosed().subscribe(() => {
      this.store.dispatch(pushDestinationBlocklist());
      this.suspendFetching = false;
    });
  }

  protected onRefresh(): void {
    this.store.dispatch(fetchDestinationBlocklist());
  }
}
