# -*- coding: utf-8 -*-



def count_path(best_path_file_path, councount_path_dif_file_path,count_file_path,th):
    best_path_file = open(best_path_file_path,'r')
    councount_path_dif_file = open(councount_path_dif_file_path,'w')

    count5_file = open(count_file_path,'w')




    #the total best path number
    num_of_path = 10

    #label the number of best path now
    count_of_path = 0

    #used to save the other n-1 best paths
    n_best_path = []

    #used to save the best path
    best_path = []
    path_length = 0



    #th = 2


    source_line = best_path_file.readline()
    while source_line :
        #print source_line
        if source_line.strip().startswith("C") and source_line.strip().endswith("1"):#this is the best path
            count_of_path = 1
            councount_path_dif_file.write("\n" + source_line.strip()[:-2] + " ")
            count5_file.write("\n" + source_line.strip()[:-2] + " ")

            #print source_line

            best_path = best_path_file.readline().split() #this is the first best path
            path_length = len(best_path)#the total length of the path

            #read the next line,in case this sentence only have one path
            source_line = best_path_file.readline()
            if source_line.strip().startswith("C") and source_line.strip().endswith("1"):
                count = [0] * path_length
                for index, i in enumerate(count):
                    councount_path_dif_file.write(str(i) + " ")
                    if i > th:count5_file.write(str(index +1) + " ")



        elif source_line.strip().startswith("C"):#this is the other best path rather than the best path
            count_of_path += 1
            path = best_path_file.readline().split()
            n_best_path.append(path)

            #read the next line, in case it is next sentence
            source_line = best_path_file.readline()

            if count_of_path == num_of_path or (source_line.strip().startswith("C") and source_line.strip().endswith("1")):
                #there are some sentecne that do not have all the n best path, start to count the other path
                count = [0] * path_length
                #print count
                for i in range(path_length):
                    for path in range(count_of_path - 1):
                        if i < len(n_best_path[path]) and n_best_path[path][i] != best_path[i]:#protect list from overstep the boundary
                            count[i] += 1

                #print count
                for index,i in enumerate(count):
                    councount_path_dif_file.write(str(i) + " ")
                    if i > th:count5_file.write(str(index +1) + " ")

                #stop count and re-initialize the paramaters
                count_of_path = 0
                n_best_path= []
                best_path = []
                path_length = 0

        else:
            source_line = best_path_file.readline()


if __name__ == '__main__':

    dir = "C:\\Users\\summer\\Desktop\\handwriting_research\\error_analysis_script1\\"

    best_path_file_path = dir + "10_best_path.txt"
    councount_path_dif_file_path = dir +"10_best_path_count_dif.txt"

    count_file_path = dir +"count5.txt"

    '''
    best_path_file_path = "C:\\Users\\summer\\Desktop\\handwriting_research\\error_analysis_script\\10_best_path.txt"
    councount_path_dif_file_path = "C:\\Users\\summer\\Desktop\\handwriting_research\\error_analysis_script\\10_best_path_count_dif.txt"

    count_file_path = "C:\\Users\\summer\\Desktop\\handwriting_research\\error_analysis_script\\count5.txt"
    '''
    th = 2

    count_path(best_path_file_path,councount_path_dif_file_path,count_file_path,th)



