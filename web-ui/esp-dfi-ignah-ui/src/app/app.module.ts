import { NgModule, isDevMode } from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';
import { BrowserAnimationsModule } from '@angular/platform-browser/animations';
import { LayoutModule } from '@angular/cdk/layout';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';

import { AppRoutingModule } from './app-routing.module';

import { MatButtonModule } from '@angular/material/button';
import { MatDialogModule } from '@angular/material/dialog';
import { MatFormFieldModule } from '@angular/material/form-field';
import { MatIconModule } from '@angular/material/icon';
import { MatInputModule } from '@angular/material/input';
import { MatListModule } from '@angular/material/list';
import { MatRadioModule } from '@angular/material/radio';
import { MatSelectModule } from '@angular/material/select';
import { MatSidenavModule } from '@angular/material/sidenav';
import { MatSliderModule } from '@angular/material/slider';
import { MatTabsModule } from '@angular/material/tabs';
import { MatToolbarModule } from '@angular/material/toolbar';

import { AppComponent } from './app.component';
import { DisplayComponent } from './components/display/display.component';
import { FreeTextComponent } from './components/display/free-text/free-text.component';
import { RunningTextComponent } from './components/display/running-text/running-text.component';
import { AdditionalVehiclesComponent } from './components/display/additional-vehicles/additional-vehicles.component';
import { StoreModule } from '@ngrx/store';
import { ModeEffects, modeFeatureKey, modeReducer } from './store/mode';
import { EffectsModule } from '@ngrx/effects';
import { HttpClientModule } from '@angular/common/http';
import { VehicleDialogComponent } from './components/display/additional-vehicles/vehicle-dialog/vehicle-dialog.component';
import { StoreDevtoolsModule } from '@ngrx/store-devtools';
import { SettingsComponent } from './components/settings/settings.component';

@NgModule({
    declarations: [AppComponent],
    imports: [
        BrowserModule,
        BrowserAnimationsModule,
        FormsModule,
        HttpClientModule,
        LayoutModule,
        ReactiveFormsModule,
        AppRoutingModule,
        MatButtonModule,
        MatDialogModule,
        MatFormFieldModule,
        MatIconModule,
        MatInputModule,
        MatListModule,
        MatRadioModule,
        MatSelectModule,
        MatSidenavModule,
        MatSliderModule,
        MatTabsModule,
        MatToolbarModule,
        StoreModule.forRoot(),
        StoreModule.forFeature(modeFeatureKey, modeReducer),
        EffectsModule.forRoot(),
        EffectsModule.forFeature([ModeEffects]),
        StoreDevtoolsModule.instrument({ maxAge: 25, logOnly: !isDevMode() }),
        DisplayComponent,
        FreeTextComponent,
        RunningTextComponent,
        AdditionalVehiclesComponent,
        VehicleDialogComponent,
        SettingsComponent
    ],
    providers: [],
    bootstrap: [AppComponent]
})
export class AppModule { }
