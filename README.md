# tsock
Application distribuée de boîtes aux lettres sur Internet, utilisant l'API socket.

Documentation : https://github.com/aguiller31/tsock/wiki

## Exemples 
Console n°1 : Emetteur
```
./tsock -b 8005
```
Console n°2 : Boîte aux lettres
```
./tsock -e 4 -n 5 -l 10 localhost 8005
```
Console n°3 : Récepteur
```
./tsock -r 4 localhost 8005
```

![emetteur](https://user-images.githubusercontent.com/128867138/227670478-7685d742-b0fe-4a8c-825c-fe3898f3bafc.png)
![BAL](https://user-images.githubusercontent.com/128867138/227670513-8b02b179-153d-4eec-b26c-a235e5b90bda.png)
![recepteur](https://user-images.githubusercontent.com/128867138/227670404-b6a98392-2979-4b53-9d42-30287228306f.png)
