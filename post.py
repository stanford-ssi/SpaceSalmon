#Added by timv to fix PIO linker issues
#Doesn't seem to be needed anymore! fun.

#Import('projenv')
# projenv.ProcessUnFlags("\
# -Ilib/SSISD/src/diskio \
# -Ilib/SSISD/src/sd_mmc \
# -Ilib/SSISD/src/FatFs/source \
# -Ilib/SSISD/src/misc \
# -Ilib/SSISD/src/include \
# -Ilib/SSISD/src/hri \
# -Ilib/SSISD/src/hal/include \
# -Ilib/SSISD/src/hpl/port \
# -Ilib/SSISD/src/hal/utils/include ")

#print("Trimmed Projenv CPPPATH list")