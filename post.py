#Added by timv to fix PIO linker issues

Import('projenv')
# projenv.ProcessUnFlags("\
# -Ilib/SSISD/src/diskio \
# -Ilib/SSISD/src/sd_mmc \
# -Ilib/SSISD/src/FatFs/source \
# -Ilib/SSISD/src/misc \
# -Ilib/SSISD/src/include \
# -Ilib/SSISD/src/hri \
# -Ilib/SSISD/src/hal/include \
# -Ilib/SSISD/src/hpl/port \
# -Ilib/SSISD/src/hal/utils/include \
# -Ilib/SSIEth/src/misc \
# -Ilib/SSIEth/src/include \
# -Ilib/SSIEth/src/hri \
# -Ilib/SSIEth/src/hal/include \
# -Ilib/SSIEth/src/hpl/port \
# -Ilib/SSIEth/src/hal/utils/include \
# -Ilib/ssi_adc/src/misc \
# -Ilib/ssi_adc/src/include \
# -Ilib/ssi_adc/src/hri \
# -Ilib/ssi_adc/src/hal/include \
# -Ilib/ssi_adc/src/hpl/port \
# -Ilib/ssi_adc/src/hal/utils/include ")

print("Trimmed Projenv CPPPATH list")