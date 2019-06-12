# cryptbreaker

![cryptbreaker](cryptbreaker_art.jpg)

Quebrando o método `crypt` da biblioteca `crypt.h`, usada para autenticação no kernel GNU Linux/Unix like de forma paralela em C.

## Build

```sh
    make
```

## Run

Adicione os hosts da rede OpenMP no arquivo `ip_list`.
Certifique-se que o seu host possui somente um único processo do `cryptbreaker` executando (`localhost N=1`).

Execute com o comando:

```sh
    make run
```

