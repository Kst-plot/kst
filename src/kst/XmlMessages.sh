function get_files
{
    echo x-kst.xml
}

function po_for_file
{
    case "$1" in
       x-kst.xml)
           echo kst_xml_mimetypes.po
       ;;
    esac
}

function tags_for_file
{
    case "$1" in
       x-kst.xml)
           echo comment
       ;;
    esac
}
