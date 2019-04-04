export LD_LIBRARY_PATH=$(pwd)/lib:$LD_LIBRARY_PATH

bin/HDRNET \
/home/ubuntu/project/ImageEnhance/hdrnet/hdrnet-mobile/pretrained_models/p20_model/binaries \
/home/ubuntu/project/ImageEnhance/hdrnet/hdrnet-mobile/sample_data/8-2.jpg
