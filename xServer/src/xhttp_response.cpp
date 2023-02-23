//
// Created by lyn on 2023/2/23.
//
#include "xhttp_response.h"

std::string xyq::xhttp_response::to_string() {
    // 写响应行
    this->buffer = this->http_version + ' ' + std::to_string(this->status_code) + ' ' + this->message + "\r\n";
    // 计算响应体长度
    size_t content_length = this->rsp_content.size();
    this->set_header("content-length", std::to_string(content_length).data());
    // 写响应头
    for (auto &kv: this->rsp_header) {
        this->buffer += kv.first + ": " + kv.second + "\r\n";
    }
    // 写空行，响应头内容结束
    this->buffer += "\r\n";
    // 写响应体
    this->buffer += this->rsp_content;
    return this->buffer;
}

xyq::xhttp_response::xhttp_response() {
    this->ok();
    this->set_header("Content-Type", "text/html;charset=UTF-8");
    this->set_header("Connection", "close");
    // this->set_header("Content-Encoding", "gzip");
}

void xyq::xhttp_response::not_found() {
    this->message = "NOT_FOUND";
    this->status_code = 404;
}

void xyq::xhttp_response::ok() {
    this->message = "OK";
    this->status_code = 200;
}

void xyq::xhttp_response::bad_request() {
    this->message = "BAD_REQUEST";
    this->status_code = 400;
}

void xyq::xhttp_response::time_out() {
    this->message = "TIME_OUT";
    this->status_code = 408;
}

void xyq::xhttp_response::set_status_code(std::string status_code) {
    int code = std::stoi(status_code);
    this->status_code = code;
}

void xyq::xhttp_response::set_status_code(int status_code) {
    this->status_code = status_code;
}

void xyq::xhttp_response::set_message(std::string message) {
    this->message = message;
}

void xyq::xhttp_response::set_header(std::string key, std::string value) {
    if (key.size() == 0 and value.size() == 0)
        return;
    this->rsp_header[key] = value;
}

void xyq::xhttp_response::operator=(const xyq::xhttp_response &rsp) {
    this->status_code = rsp.status_code;
    this->message = rsp.message;
    this->rsp_content = rsp.rsp_content;
    this->rsp_header = rsp.rsp_header;
}

void xyq::xhttp_response::set_content(std::string content) {
    this->rsp_content = content;
}

void xyq::xhttp_response::add_content(std::string content) {
    this->rsp_content += content;
}