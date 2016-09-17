# -*- coding: gb18030 -*-

from  best_path_count import *
from  best_path_extract import *

def get_continue(error_list):
    list = []
    for num in error_list: list.append(num)
    single_pos = []
    continue_pos = []
    if len(list) == 0:
        return single_pos,continue_pos
    elif len(list) == 1:
        return list,continue_pos
    else:
        if list[1] - list[0] == 1:
            continue_pos = list
        else:
            single_pos.append(list[0])
            del list[0]
            new_single_pos,continue_pos = get_continue(list)
            single_pos += new_single_pos
        return single_pos,continue_pos




class Sentence_error:
    name = ""
    error_pos = []
    single_error_pos = []
    continue_error_pos = []
    predict = []
    def __init__(self,line):
        sentence = line.split()
        self.name = sentence[0]
        del sentence[0]
        self.error_pos = []
        self.predict = []
        if len(sentence) != 0 :
            for num in sentence: self.error_pos.append(int(num))
        self.single_error_pos,self.continue_error_pos = get_continue(self.error_pos)
        #get the predict error position

        self.predict = self.single_error_pos
        if len(self.continue_error_pos) >=4:
            self.predict += self.continue_error_pos[:2] + self.continue_error_pos[-2:]
            #self.predict.append(self.continue_error_pos[0]-1)
            #self.predict.append(self.continue_error_pos[0]-2)
            #self.predict.append(self.continue_error_pos[-1]+1)
            #self.predict.append(self.continue_error_pos[-1]+2)
            #self.predict.append(self.predict[0]-1)
            #self.predict.append(self.predict[0]-2)

class Error_file:
    data = []
    def __init__(self,filepath):
        self.data = []
        file = open(filepath,'r')
        line = file.readline()
        while line :
            if not line.startswith("t"):
                sentence = Sentence_error(line)
                self.data.append(sentence)
            line = file.readline()

    def get(self,name):
        for sentence in self.data:
            if sentence.name == name:
                return sentence
        return False

if __name__ == '__main__':

    #get dir
    analysis_dir = "C:\\Users\\summer\\Desktop\\handwriting_research\\error_analysis_script1\\"
    error_count_dir = "C:\\Users\\summer\\Desktop\\handwriting_research\\error_count1\\"
    #****************************************************************************
    #extract the best path
    source_file=analysis_dir +  "10best.lats"
    extract_file = analysis_dir+ "10_best_path.txt"
    acoutic_score_file = analysis_dir + "10-best_path_acoustic_score.txt"
    lm_score_file = analysis_dir + "10-best_path_lm_score.txt"
    extract_best_path(source_file,extract_file,acoutic_score_file,lm_score_file)
    #****************************************************************************
    #count the best path
    best_path_file_path = analysis_dir + "10_best_path.txt"
    councount_path_dif_file_path = analysis_dir +"10_best_path_count_dif.txt"
    count_file_path = analysis_dir +"count5.txt"
    #set the threshold for each word of count
    th = 2
    #count the difference in best path file
    count_path(best_path_file_path,councount_path_dif_file_path,count_file_path,th)
    #delete the empty line in the raw count file, and save the deleted file to count_result.txt
    qfile = open('count_result.txt','w').writelines([l for l in open('count5.txt','r').readlines() if l[:-1].strip()])
    #****************************************************************************
    #open all the result file to compare the predict result
    substitute_error_file = Error_file(error_count_dir + 'substitute_error_result.txt')
    delete_error_file = Error_file(error_count_dir+ 'delete_error_result.txt')
    count5_file = Error_file(analysis_dir + 'count_result.txt')


    predict_num = 0
    total_single_pos = 0
    total_sentence = 0
    for path in count5_file.data:
        name = path.name
        total_single_pos += len(path.predict)
        if substitute_error_file.get(name) != False:
            total_sentence += 1
            substitute_error = substitute_error_file.get(name)
            delete_error = delete_error_file.get(name)
            for word in path.predict:
                if (word in delete_error.error_pos) or (word in substitute_error.error_pos):
                    predict_num += 1

    print predict_num
    print total_single_pos
    print str((1-float(predict_num) / total_single_pos) * 100) + "%"
    print total_sentence





'''
    name = 'C004-P17_5'

    print count5_file.get(name).error_pos
    print count5_file.get(name).single_error_pos
    print count5_file.get(name).continue_error_pos
'''