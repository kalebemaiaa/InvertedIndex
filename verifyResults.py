def gabarito(filename, gabaritoFile):
    def parse_line(line):
        # Remove colchetes e espaços desnecessários e separa por vírgulas
        line = line.strip().strip("[]")
        if line:
            return [int(x.strip()) for x in line.split(",") if x.strip()]
        return []

    with open(filename, "r") as f, open(gabaritoFile, "r") as g:
        data_f = [parse_line(row) for row in f]
        data_g = [parse_line(row) for row in g]

    return data_f, data_g

def compareLists(list1, list2):
    # Verifica se as listas têm o mesmo comprimento
    if len(list1) != len(list2):
        raise ValueError("As listas não têm o mesmo comprimento")
    
    # Lista para armazenar a porcentagem de itens semelhantes
    percentages = []

    for sublist1, sublist2 in zip(list1, list2):
        # Conjunto para facilitar a comparação
        set1 = set(sublist1)
        set2 = set(sublist2)
        
        # Calcular o número de itens semelhantes
        common_items = len(set1 & set2)
        total_items = len(set1 | set2)
        
        # Calcular a porcentagem de itens semelhantes
        if total_items == 0:
            percentage = 0.0
        else:
            percentage = (common_items / total_items) * 100
        
        percentages.append(percentage)
    
    return percentages

# Exemplo de uso
file_data, gabarito_data = gabarito("./results/resultsPython.txt", "./results/resultsCPP.txt")
percentages = compareLists(file_data, gabarito_data)

# Exibir os resultados
for i, percentage in enumerate(percentages):
    print(f"Lista {i}: {percentage:.2f}% semelhante")
