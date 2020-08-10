module ApplicationHelper
  def heda_path(str)
    raw('127.0.0.1:8083/' + str)
  end

  def post_heda_cmd(cmd)
    raw('$.post("127.0.0.1:8083/run?cmd=' + cmd + '")')
  end

  def onclick_heda_cmd(cmd)
    raw("onclick='"+post_heda_cmd(cmd)+"'")
  end
end
