local citizen = {}

function citizen.checkIfLogin(connectId) 
    if nil == loggedCitizens[connectId] then
        return false
    end
    return true
end

function CtrLogin(connectId, email, password)
    LogI("hi")
    LogI(connectId.. " connectId")
    LogI(email.." email")
    LogI(password.." password")

    if nil == email or nil == password then
        NTAddReplyRawString(connectId, nil, nil, "-请输入用户名或密码\r\n")
        return
    end

    local citizen = util.db:select(
    'b_citizen', 
    'citizen_id,code,nickname,email', 
    {email=email, password=md5.sumhexa(password)})

    if not citizen[0] then
        NTAddReplyRawString(connectId, nil, nil, "-用户名或密码错误哈哈哈\r\n")
        return
    end
    citizen = citizen[0]

    loggedCitizens[connectId] = citizen
    NTAddReplyRawString(connectId, nil, nil, "+登录成功\r\n")

end

return citizen
