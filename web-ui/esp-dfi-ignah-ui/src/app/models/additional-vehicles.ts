export interface AdditionalVehicle {
  lineNumber: string;
  destination: string;
  departure: string;
}

export interface AdditionalVehicles {
  vehicles: AdditionalVehicle[];
}