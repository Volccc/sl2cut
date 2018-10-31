**sl2cut** - console application to cut .sl2 files


Command:


*sl2cut file.sl2*

only print info about this log file

**Possible options:**

*-c start:finish* start and end sounding to output. You may find it in Lowrance Sonar Log Viewer

*-d [PTDS]* - which channels to delete

- P - remove primary channel
- T - remove secondary channel
- D - remove DSI channel
- S - remove SideScan channels: Left, Right, Composite

*-a* - anonymize log: output log coordinates will be shifted with constants (hardcoded in utility)

*-n* - define northing shift
*-e* - define easting shift

Output file name is input name + _start_end

**Example:**

*sl2cut -c 2000:10000 -n 46.45 -e 6.5 -d PTD sonar01.sl2*

Output file sonar01_2000-1000.sl2 will contain sounding from 2000 to 10000, primary, secondary and DSI channels will be removed (only Sidescan in output). Track will start in Geneva lake.
