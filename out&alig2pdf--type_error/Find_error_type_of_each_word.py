# -*- coding: gb18030 -*-


out_file= open(r'C:\Users\summer\Desktop\handwriting_research\error_count\out.txt', 'r')
result_file = open(r'C:\Users\summer\Desktop\handwriting_research\error_count\add_delete_result.txt','w')

out_line = out_file.readline()
while out_line :
    while out_line.find("       ") != -1:
        out_line = out_line.replace("       "," 0xFFFF")
    result_file.write(out_line)
    #result_file.write('\n')
    out_line = out_file.readline()


add_delete_file = open(r'C:\Users\summer\Desktop\handwriting_research\error_count\add_delete_result.txt', 'r')
#add_delete_file = open(r'F:\python\python_learn\pycharm\Script_data\add_more.txt', 'r')
del_file = open(r'C:\Users\summer\Desktop\handwriting_research\error_count\delete_error_result.txt','w')
ins_file = open(r'C:\Users\summer\Desktop\handwriting_research\error_count\insert_error_result.txt','w')
sub_file = open(r'C:\Users\summer\Desktop\handwriting_research\error_count\substitute_error_result.txt','w')

def write_to_file(sentence_label, error_index, error_file):
    error_file.write(sentence_label + " ")
    for index in error_index:
        error_file.write(str(index) + " ")
    error_file.write("\n")

def num_of_0x0000(words,index):
    return words[0:index].count("0xFFFF")

del_num = 0
ins_num = 0
sub_num = 0

line = add_delete_file.readline()
while line:
    words = line.split()
    del_wrong = []
    ins_wrong = []
    sub_wrong = []

    sentence_label = words[2].split(".")[0]  # get the label of the sentence
    #print sentence_label
    line = add_delete_file.readline()#read the next line: LAB
    LAB = line.split()
    del LAB[0]
    line = add_delete_file.readline()#read the next line: REC
    REC = line.split()
    #print REC
    if len(REC) == 0:
        print "error: len(REC)=0"
        break
    del REC[0]
    #print LAB
    #print REC
    if len(LAB)!= len(REC):
        print "stop in: "+ sentence_label
        break
    for index in range(len(LAB)):
        if LAB[index] != REC[index]:
            if LAB[index] == "0xFFFF":
                ins_wrong.append(index+1-num_of_0x0000(REC,index))
            else:
                if REC[index] == "0xFFFF":
                    del_wrong.append(index+1-num_of_0x0000(REC,index))
                else:
                    sub_wrong.append(index+1-num_of_0x0000(REC,index))
    write_to_file(sentence_label,ins_wrong,ins_file)
    write_to_file(sentence_label,del_wrong,del_file)
    write_to_file(sentence_label,sub_wrong,sub_file)
    del_num += len(del_wrong)
    ins_num += len(ins_wrong)
    sub_num += len(sub_wrong)
    #print "sentence label: " + sentence_label
    #print "insert error: " + str(ins_wrong)
    #print "delete error: " + str(del_wrong)
    #print "substitute error " + str(sub_wrong)
    line = add_delete_file.readline()  # read the next line:

ins_file.write("total ins error: "+ str(ins_num))
del_file.write("total del error: "+ str(del_num))
sub_file.write("total sub error: "+ str(sub_num))

