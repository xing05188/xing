const rooms = [
    { num: "405", x: 531, y: 813, img: ["images/405.jpg"], desc: "南边🚹男卫生间🚾" },
    { num: "406", x: 423, y: 941, img: ["images/406.jpg"], desc: "南边🚺女卫生间🚾" },
    { num: "407", x: 479, y: 796, img: ["images/407.jpg"], desc: "陈梁 严海洲 叶珂男<br>🧑‍🎓研究生工作室🧑‍💻" },
    { num: "408", x: 380, y: 923, img: ["images/408.jpg"], desc: "大小姐驾到，通通闪开！😘" },
    { num: "409", x: 404, y: 733, img: ["images/409.jpg","images/409_1.jpg"], desc: "陈梁 朱亚萍 代玉琢<br>409左 🧑‍🎓研究生工作室🧑‍💻<br>陈梁 杜庆峰 韩永琦<br>409右 🧑‍🎓研究生工作室🧑‍💻" },
    { num: "410", x: 327, y: 879, img: ["images/410.jpg"], desc: "王冬青 李江峰 夏波涌 张颖<br>🧑‍🏫教研室🛖" },
    { num: "412", x: 272, y: 833, img: ["images/412.jpg"], desc: "刘岩 张惠娟 孙萍 罗怡桂<br>🧑‍🏫教研室🛖" },
    { num: "414", x: 114, y: 600, img: ["images/414.jpg"], desc: "软件学院介绍室🏢" },
    { num: "415", x: 273, y: 598, img: ["images/415.jpg"], desc: "⚡强电间⚡" },
    { num: "416", x: 114, y: 467, img: ["images/416.jpg","images/416_1.jpg"], desc: "陈梁 杨旻<br>🖥️教学机房🏢" },
    { num: "417", x: 273, y: 466, img: ["images/417.jpg"], desc: "会议室🗣️" },
    { num: "418", x: 114, y: 328, img: ["images/418.jpg","images/418_1.jpg","images/418_2.jpg","images/418_3.jpg"], desc: "<br>陈梁 尹长青 陈韩悦 袁时金<br>418右 🧑‍🎓研究生工作室🧑‍💻<br>张林 江建慧 刘琴<br>418左 教授办公室🏢" },
    { num: "419", x: 273, y: 326, img: ["images/419.jpg","images/419_1.jpg","images/419_2.jpg"], desc: "陈梁 张晶<br>教学实验室🔬<br>同济大学软件学院软件技术研究中心🔬<br>计算机系统实验室🔬" },
    { num: "420", x: 84, y: 176, img: ["images/420.jpg"], desc: "电梯旁小房间🏢" },
    { num: "426", x: 231, y: 67, img: ["images/426.jpg","images/426_1.jpg"], desc: "陈梁 杨旻<br>🖥️教学机房🏢" },
    { num: "428", x: 299, y: 67, img: ["images/428.jpg","images/428_1.jpg"], desc: "杨旻 严海洲<br>网络与服务器管理室🖥️" },
    { num: "430", x: 380, y: 103, img: ["images/430.jpg","images/430_1.jpg","images/430_2.jpg"], desc: "陈梁 杨旻<br>🖥️多媒体教学机房🏢" },
    { num: "432", x: 578, y: 89, img: ["images/432.jpg"], desc: "同济大学党员之家🚩" },
    { num: "434", x: 617, y: 89, img: ["images/434.jpg"], desc: "讲课教室🏢" },
    { num: "441", x: 712, y: 315, img: ["images/441.jpg"], desc: "会议室🗣️" },
    { num: "442", x: 596, y: 256, img: ["images/442.jpg","images/442_1.jpg","images/442_2.jpg"], desc: "闫鹏 张晶 林伊凡 张晓雅 钱银飞 王昊榕 俞晓静<br>442右 学院办公室🏢<br>刘梦露 李慧敏 王彩霞 杨丹 姚仕仪<br>442左 教务办公室🏢" },
    { num: "443", x: 693, y: 416, img: ["images/443.jpg"], desc: "陈梁 杨旻 严海洲<br>实验中心🔬" },
    { num: "444", x: 574, y: 373, img: ["images/444.jpg","images/444_1.jpg"], desc: "档案室🏢" },
    { num: "446", x: 578, y: 421, img: ["images/446.jpg","images/446_1.jpg"], desc: "张砚秋 丁瑞庭 葛蕾 焦嘉欣 钟梦莹 陈璞皎<br>🧑‍🎓学生工作办公室/学工办🏢" },
    { num: "448", x: 586, y: 489, img: ["images/448.jpg"], desc: "王彩霞 杨丹<br>未知❓" },
    { num: "450", x: 620, y: 550, img: ["images/450.jpg","images/450_1.jpg","images/450_2.jpg","images/450_3.jpg","images/450_4.jpg"], desc: "申恒涛<br>450右 院长办公室🏢<br>熊岚<br>450左 党委书记办公室🏢" },
    { num: "451", x: 747, y: 503, img: ["images/451.jpg","images/451_1.jpg","images/451_2.jpg"], desc: "王成 何良华 张林<br>副院长办公室🏢" },
    { num: "455", x: 886, y: 586, img: ["images/455.jpg"], desc: "会议室🗣️" },
    { num: "456", x: 819, y: 678, img: ["images/456.jpg"], desc: "周微微 陆凤兰 赵清理<br>党委办公室🏢" },
    { num: "998", x: 470, y: 227, img: ["images/998.jpg"], desc: "象棋🐘 国际围棋♟️ 钢琴🎹" },
    { num: "999", x: 596, y: 915, img: ["images/999.jpg","images/999_1.jpg"], desc: "台球🎱 乒乓球🏓" }
    
];
const mapContainer = document.querySelector('.map-container');
rooms.forEach(room => {
    const btn = document.createElement('button');
    btn.className = 'room-dot';
    btn.style.left = `${room.x}px`;
    btn.style.top = `${room.y}px`;
    btn.title = room.num;
    if(room.num=="998"||room.num=="999")
        btn.innerHTML = `<div class="dot"></div>`;
    else
        btn.innerHTML = `<div class="dot"></div><span class="label">${room.num}</span>`;
    btn.onclick = () => showRoom(room.num);
    mapContainer.appendChild(btn);
});

const mapImg = document.getElementById('map-img');
let offsetX = 0, offsetY = 0;
let dragging = false, startX = 0, startY = 0;

mapImg.style.transformOrigin = "0 0";
mapImg.style.transform = "translate(0px, 0px)";

mapImg.addEventListener('mousedown', function(e) {
    dragging = true;
    startX = e.clientX;
    startY = e.clientY;
    mapImg.style.cursor = "grabbing";
});
document.addEventListener('mousemove', function(e) {
    if (!dragging) return;
    offsetX += e.clientX - startX;
    offsetY += e.clientY - startY;
    startX = e.clientX;
    startY = e.clientY;
    updateMapTransform();
});
document.addEventListener('mouseup', function() {
    dragging = false;
    mapImg.style.cursor = "grab";
});
function updateMapTransform() {
    mapImg.style.transform = `translate(${offsetX}px, ${offsetY}px)`;
    document.querySelectorAll('.room-dot').forEach((btn, idx) => {
        btn.style.transform = `translate(${offsetX}px, ${offsetY}px)`;
    });
}
document.querySelectorAll('.room-dot').forEach(btn => {
    btn.style.transform = `translate(0px, 0px)`;
});

function syncRoomDots() {
    document.querySelectorAll('.room-dot').forEach((btn, idx) => {
        btn.style.transform = `translate(${offsetX}px, ${offsetY}px)`;
    });
}
document.addEventListener('mousemove', syncRoomDots);
document.addEventListener('mouseup', syncRoomDots);

const searchInput = document.getElementById('search-content');
const searchBtn = document.getElementById('search-btn');
searchBtn.addEventListener('click', function () {
    const val = searchInput.value.trim();
    if (val) {
        const found = rooms.find(r => r.num.includes(val) || r.desc.includes(val));
        if (found) showRoom(found.num);
        else alert('我是🤡，未找到相关房间💔');
    }
});

const modalBg = document.getElementById('modal-bg');
const closeBtn = document.getElementById('close-btn');
const leftArrow = document.getElementById('left-arrow');
const rightArrow = document.getElementById('right-arrow');
const roomImg = document.getElementById('room-img');
const roomTitle = document.getElementById('room-title');
const roomDesc = document.getElementById('room-desc');

let currentIndex = -1;

function showRoom(roomNum) {
    currentIndex = rooms.findIndex(r => r.num === roomNum);
    if (currentIndex === -1) return;
    const room = rooms[currentIndex];
    const imgBox = document.getElementById('room-img-box');
    imgBox.innerHTML = '';
    (room.img || []).forEach(src => {
        const img = document.createElement('img');
        img.src = src;
        img.alt = room.num + "图片";
        img.className = "room-img";
        imgBox.appendChild(img);
    });
    if(room.num=="998")
        roomTitle.textContent="学生活动区域🎆";
    else if(room.num=="999")
        roomTitle.textContent="教工小家🎇";
    else
        roomTitle.textContent = "🏢 "+room.num + " 房间 🏢";
    roomDesc.innerHTML = room.desc;
    modalBg.style.display = 'flex';
    leftArrow.disabled = (currentIndex === 0);
    rightArrow.disabled = (currentIndex === rooms.length - 1);
    closeBtn.focus();
}

closeBtn.onclick = () => {
    modalBg.style.display = 'none';
    roomImg.src = "";
};
modalBg.onclick = e => {
    if (e.target === modalBg) modalBg.style.display = 'none';
};
document.addEventListener('keydown', e => {
    if (modalBg.style.display === 'flex') {
        if (e.key === "Escape") modalBg.style.display = 'none';
        if (e.key === "ArrowLeft") leftArrow.click();
        if (e.key === "ArrowRight") rightArrow.click();
    }
});
leftArrow.onclick = () => {
    if (currentIndex > 0) showRoom(rooms[currentIndex - 1].num);
};
rightArrow.onclick = () => {
    if (currentIndex < rooms.length - 1) showRoom(rooms[currentIndex + 1].num);
};