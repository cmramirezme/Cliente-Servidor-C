#Complia el archivo p2-dogServer.c
server: p2-dogServer.c
	gcc p2-dogServer.c -o dogServer -pthread
	./dogServer
client: p2-dogClient.c
	gcc p2-dogClient.c -o dogClient
	./dogClient
Reiniciar_Todo: 
	./Modificar_Num_Registros
	./Inicializar_Hash
	touch dataDogs.dat
generador: generador.c
	gcc generador.c -o generador
	./generador
Mod_Num_Regs: 
	./Modificar_Num_Registros
Valor_Hash: Valor_Hash.c
	gcc Valor_Hash.c -o Valor_Hash
	./Valor_Hash
