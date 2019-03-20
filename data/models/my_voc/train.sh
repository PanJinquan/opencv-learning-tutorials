# 
./darknet detector train my_voc/voc.data my_voc/voc.cfg my_voc/darknet19_448.conv.23


# 
./darknet partial my_voc/voc.cfg my_voc/yolov3.weights darknet19_448.conv.23 23


