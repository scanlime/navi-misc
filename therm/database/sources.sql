--
-- An example file to initialize the 'sources' table
--

USE therm;

INSERT INTO sources (medium, protocol, station_id, name, description)
  VALUES( 'usb', 0, 0,
          'base_station',
          'Located in the base station unit, mounted behind the couch in the Apartment 2 living room.' );

INSERT INTO sources (medium, protocol, station_id, name, description)
  VALUES( 'rf', 0, 1,
          'prototype',
          'The prototype unit, used for development and debugging purposes.' );

INSERT INTO sources (medium, protocol, station_id, name, description)
  VALUES( 'rf', 0, 2,
          'apt2_living_room',
          'Located near the thermostat in the Apartment 2 living room.' );

INSERT INTO sources (medium, protocol, station_id, name, description)
  VALUES( 'rf', 0, 3,
          'navi',
          'Located near Navi in Apartment 2\'s downstairs closet.' );

--- The End ---
