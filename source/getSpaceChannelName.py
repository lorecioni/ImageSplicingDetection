def getSpaceChannelName(space,channel):
    nameSpace = ""
    nameChannel = ""
    if space == 0:
        nameSpace = "HSV"
        if channel == 0:
            nameChannel = "V"
        elif channel == 1:
            nameChannel = "S"
        elif channel == 2:
            nameChannel = "H"
        elif channel == 3:
            nameChannel = "full"
    elif space == 1:
        nameSpace = "RGB"
        if channel == 0:
            nameChannel = "B"
        elif channel == 1:
            nameChannel = "G"
        elif channel == 2:
            nameChannel = "R"
        elif channel == 3:
            nameChannel = "full"
    elif space == 2:
        nameSpace = "YCbCr"
        if channel == 0:
            nameChannel = "Cr"
        elif channel == 1:
            nameChannel = "Cb"
        elif channel == 2:
            nameChannel = "Y"
        elif channel == 3:
            nameChannel = "full"
    elif space == 4:
        nameSpace = "Lab"
        if channel == 0:
            nameChannel = "b"
        elif channel == 1:
            nameChannel = "a"
        elif channel == 2:
            nameChannel = "L"
        elif channel == 3:
            nameChannel = "full"

    return (nameSpace,nameChannel)