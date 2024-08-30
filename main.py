import re 
import os

stop_words = []
pontuation = r"[.,;:!?«»()\[\]]"

def clean_word(word: str) -> str:
    cleaned_word = re.sub(pontuation, " ", word).lower()
    return cleaned_word if cleaned_word not in stop_words else None

def words(word: str)->list[str]:
    cleaned_s = re.sub(pontuation, "", word)
    return [w for w in cleaned_s.lower().split() if w not in stop_words]

# Caso seja um arquivo do dhbb
def remove_sections(text: str) -> str:
    return re.sub(r'---.*?---', '', text, flags=re.DOTALL)

class InvertedIndex:
    def __init__(self):
        self.__list_words = {}
        self.__list_idDoc = []
    
    def __pre_proc(self, txt: str, idDoc: int):
        return [(w, idDoc) for w in words(txt)]
    
    def __pre_proc_pairs(self, txt: str, idDoc: int):
        word_list = words(txt)
        pairs = []
        for i in range(len(word_list) - 1):
            pair = f"{word_list[i]} {word_list[i + 1]}"
            pairs.append((pair, idDoc))
        return pairs

    def __ordered_insert(self, num: int, lst: list[int]):
        if not lst:
            return [num]
        for i, b in enumerate(lst):
            if num == b:
                return lst
            elif num < b:
                return lst[:i] + [num] + lst[i:]
        return lst + [num]

    def __insert_word(self, word: str, idDoc: int):
        if word not in self.__list_words:
            self.__list_words[word] = []
        
        self.__list_idDoc = self.__ordered_insert(idDoc, self.__list_idDoc)
        self.__list_words[word] = self.__ordered_insert(idDoc, self.__list_words[word])
    
    def __index_doc(self, pairs:tuple[str, int]):
        '''
            pairs: uma tupla que contem a palavra a ser indexada e o id do documento.
        '''
        for pair in pairs:
            self.__insert_word(pair[0], pair[1])

    def merge(self, l1, l2, operation = 'and'):
        merged = []
        i, j = 0, 0
        while i < len(l1) and j < len(l2):
            if l1[i] == l2[j]:
                merged.append(l1[i])
                i += 1
                j += 1
            elif l1[i] < l2[j]:
                if operation == 'or': merged.append(l1[i])
                i += 1
            else:
                if operation == 'or': merged.append(l2[j])
                j += 1
        if operation == 'or':
            merged += l1[i:]
            merged += l2[j:]
        return merged

    
    def index_file(self, fname: str, biword = False):
        with open(fname, "r", encoding="utf-8") as f:
            text = f.read()
            text = remove_sections(text)
            if biword:
                self.__index_doc(self.__pre_proc_pairs(text))
                self.biword = biword
            else:
                self.__index_doc(self.__pre_proc(text, int(fname.split("\\")[1].split(".")[0])))

    def index_folder(self, path, biword= False):
        for fname in os.listdir(path):
            file_path = os.path.join(path, fname)
            self.index_file(file_path, biword)

    def query(self, word: str, toPrint: bool = False):
        word = clean_word(word)
        if word is None: 
            return []
        list_words = self.__list_words.get(word, [])
        if toPrint:
            print(f"{word}: -> {list_words}")
            
        return list_words
    
    def complemento(self, list_idDocs):
        return [doc_id for doc_id in self.__list_idDoc if doc_id not in list_idDocs]


class Query:
    def __init__(self, term = None, operation = None, negacao = False):
        self.term = term
        self.operation = operation
        self.negacao = negacao

    @staticmethod
    def word_(word: str):
        return Query(term = word)

    @staticmethod
    def and_(*querys):
        return Query(term = [q for q in querys], operation='and')

    @staticmethod
    def or_(*querys):
         return Query(term = [q for q in querys], operation='or')
    
    @staticmethod
    def not_(Q1: 'Query'):
        return Query(term = Q1, negacao=True)

def eval_query(query_obj: Query, idx: InvertedIndex):
    if query_obj.negacao:
        return idx.complemento(eval_query(query_obj.term, idx))
    if isinstance(query_obj.term, str):
        return idx.query(query_obj.term)
    else:
        results = [eval_query(q, idx) for q in query_obj.term]
        if not results: return []

        merged = results[0]
        for r in results[1:]:
            if not merged: return []
            merged = idx.merge(merged, r, query_obj.operation)
        return merged
    
def eval_list(queries, idx, outFile = "out.txt"):
    with open(outFile, "w") as f:
        for q in queries:
            result = eval_query(q, idx)
            f.write(str(result)+"\n")

def main():
    ivt = InvertedIndex()
    ivt.index_folder("./raw")
    queries = list()

    queries.append(Query.word_("senador"))
    queries.append(Query.not_(Query.or_(Query.word_("estudou"), Query.word_("candidato"))))
    eval_list(queries, ivt, "./results/resultsPython.txt")
    
main()