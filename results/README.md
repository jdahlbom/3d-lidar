Coordinate mapping from serial output file
==========================================

- Store the serial output as "raw-YYYY-mm-dd-hhMM.csv"
- Grep for "^<jobID>," where jobId is the id you gave the scan result set.
- Save as "YYYY-mm-dd-hhMM.csv"  ( `grep "^<jobID>," raw-YYYY-mm-dd-hhMMcsv YYYY-mm-dd-hhMM.csv` )
- run "python3 coord-mapping.py YYYY-mm-dd-hhMM.csv > cartesian-YYYY-mm-dd-hhMM.csv"
- TODO: Convert the cartesian csv data into LAS file format for viewing

