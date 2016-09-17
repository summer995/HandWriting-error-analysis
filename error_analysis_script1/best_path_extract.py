# -*- coding: gb18030 -*-

def extract_best_path(source_file_path,extract_file_path,acoustic_score_file_path,lm_score_file_path):

    source_file= open(source_file_path, 'r')
    extract_file = open(extract_file_path,'w')
    acoustic_score_file = open(acoustic_score_file_path,'w')
    lm_score_file = open(lm_score_file_path,'w')

    source_line = source_file.readline()
    while source_line :
        if source_line.startswith("C"):
            #print source_line
            extract_file.write("\n")
            extract_file.write(source_line)
            acoustic_score_file.write("\n")
            acoustic_score_file.write(source_line)
            lm_score_file.write("\n")
            lm_score_file.write(source_line)
        else:
            extract_line = source_line.split()
            #print extract_line
            if len(extract_line)>3:
                extract_file.write(extract_line[2] + " ")
                acoustic_score_file.write(extract_line[3].split(',')[1] + " ")
                lm_score_file.write(extract_line[3].split(',')[0] + " ")
        source_line = source_file.readline()



if __name__ == '__main__':
    best_path_dir = "C:\\Users\\summer\\Desktop\\handwriting_research\\error_analysis_script1\\"
    source_file=best_path_dir +  "10best.lats"
    extract_file = best_path_dir+ "10_best_path.txt"
    acoutic_score_file = best_path_dir + "10-best_path_acoustic_score.txt"
    lm_score_file = best_path_dir + "10-best_path_lm_score.txt"
    extract_best_path(source_file,extract_file,acoutic_score_file,lm_score_file)