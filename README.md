# dcf77-decoder

[DCF77](https://en.wikipedia.org/wiki/DCF77) is a German longwave station that transmits 24/7 on 77.5 kHz date and time.

Input data must be a **float32 single channel wave file**. Sample rate value is not critical. Data must be clean else the decoder won't work : You can use **Audacity** and use **Limiter** effect to remove spikes.

If you haven't a SDR that is capable to go down to 77.5 kHz, you can use the websdr of [University of Twente](https://www.utwente.nl/en/) and use the recording function : http://websdr.ewi.utwente.nl:8901/

This program takes 2 arguments :
* Input .wav location
* "1" to show decoded bits during decoding pass (optional, is disabled by default)


Build the program : 
```
g++ -Wall -pipe -Ofast main.cpp -o dcf77decoder
```


Run the program :
```
./dcf77decoder dcf77.wav
```

Output :
```
New minute detected
2022/01/01 (Saturday) - 20:38 UTC+1

New minute detected
2022/01/01 (Saturday) - 20:39 UTC+1

New minute detected
2022/01/01 (Saturday) - 20:40 UTC+1

New minute detected
2022/01/01 (Saturday) - 20:41 UTC+1

New minute detected
2022/01/01 (Saturday) - 20:42 UTC+1

New minute detected
2022/01/01 (Saturday) - 20:43 UTC+1

New minute detected
```

With displaying of decoded bits, output will be :

```
New minute detected
[0]=0, [1]=0, [2]=1, [3]=0, [4]=1, [5]=1, [6]=1, [7]=1, [8]=1, [9]=0, [10]=0, [11]=0, [12]=0, [13]=0, [14]=0, [15]=0, [16]=0, [17]=0, [18]=1, [19]=0, [20]=1, [21]=0, [22]=0, [23]=0, [24]=1, [25]=1, [26]=1, [27]=0, [28]=1, [29]=0, [30]=0, [31]=0, [32]=0, [33]=0, [34]=1, [35]=1, [36]=1, [37]=0, [38]=0, [39]=0, [40]=0, [41]=0, [42]=0, [43]=1, [44]=1, [45]=1, [46]=0, [47]=0, [48]=0, [49]=0, [50]=0, [51]=1, [52]=0, [53]=0, [54]=0, [55]=1, [56]=0, [57]=0, [58]=0
2022/01/01 (Saturday) - 20:38 UTC+1

New minute detected
[0]=0, [1]=0, [2]=1, [3]=1, [4]=1, [5]=1, [6]=1, [7]=0, [8]=0, [9]=0, [10]=1, [11]=1, [12]=1, [13]=1, [14]=1, [15]=0, [16]=0, [17]=0, [18]=1, [19]=0, [20]=1, [21]=1, [22]=0, [23]=0, [24]=1, [25]=1, [26]=1, [27]=0, [28]=0, [29]=0, [30]=0, [31]=0, [32]=0, [33]=0, [34]=1, [35]=1, [36]=1, [37]=0, [38]=0, [39]=0, [40]=0, [41]=0, [42]=0, [43]=1, [44]=1, [45]=1, [46]=0, [47]=0, [48]=0, [49]=0, [50]=0, [51]=1, [52]=0, [53]=0, [54]=0, [55]=1, [56]=0, [57]=0, [58]=0
2022/01/01 (Saturday) - 20:39 UTC+1

[...]
```
